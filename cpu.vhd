-- cpu.vhd: Simple 8-bit CPU (BrainLove interpreter)
-- Copyright (C) 2021 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Andrei Shchapaniak, xshcha00
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- ram[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_WREN  : out std_logic;                    -- cteni z pameti (DATA_WREN='0') / zapis do pameti (DATA_WREN='1')
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA obsahuje stisknuty znak klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna pokud IN_VLD='1'
   IN_REQ    : out std_logic;                     -- pozadavek na vstup dat z klavesnice
   
   -- vystupni port
   OUT_DATA : out std_logic_vector(7 downto 0);   -- zapisovana data
   OUT_BUSY : in std_logic;                       -- pokud OUT_BUSY='1', LCD je zaneprazdnen, nelze zapisovat,  OUT_WREN musi byt '0'
   OUT_WREN : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is
	-- PC --
	signal pc_reg : std_logic_vector(11 downto 0); -- 11 bits register pointer for program counter
	signal pc_inc : std_logic;                     -- signal for pointer incrementing
	signal pc_dec : std_logic;                     -- signal for pointer decrementing 
	--------
	
	-- CNT --
	signal cnt_reg : std_logic_vector(11 downto 0); -- 12 bits register for bracket counting
	signal cnt_inc : std_logic;                     -- signal for count incrementing
	signal cnt_dec : std_logic;                     -- signal for count decrementing 
	---------
	
	-- PTR --
	signal ptr_reg : std_logic_vector(9 downto 0); -- 9 bits register for data address
	signal ptr_inc : std_logic;                    -- signal for address incrementing
	signal ptr_dec : std_logic;                    -- signal for address decrementing
	---------
	
	-- MUX --
	signal mux_output : std_logic_vector(7 downto 0) := "00000000"; -- the value for writing to the memeory
	signal mux_select : std_logic_vector(1 downto 0) := "11";       -- the selector of the value
  	---------
 
	-- FSM --
	type fsm_state is (
		s_start,  -- state for reliability
		s_fetch,  -- instruction read 
		s_decode, -- instruction decoding
		
		s_change_ptr, -- '>'/'<' inc/dec pointer
		
		-- '+' / '-' --
		s_data_read,
		s_change_value,
		s_data_write,
		---------------
		
		-- '[' --
		s_loop_start_1, -- check the value
		s_loop_start_2, -- check the value of bracket counter
		s_loop_start_3, -- check the data from ROM
		---------
		
		-- ']' --
		s_loop_end_1, -- check the value
		s_loop_end_2, -- check the value of bracket counter
		s_loop_end_3, -- check the data from ROM
		s_loop_end_4, -- check the value of bracket counter again
		---------
		
		-- '~' --
		s_break_1, -- update CODE_ADDR
		s_break_2, -- check the value of bracket counter
		s_break_3, -- check the data from ROM
		---------
		
		-- '.' --
		s_print_sym, -- wait or print
		---------
		
		-- ',' --
		s_getchar_rq,   -- request for input and read input data
		s_getchar_scan, -- wait or write data from input
		---------
		
		s_null, -- null
		s_comments -- other symbols
	);
	
	signal fsm_a_state : fsm_state := s_start; -- actual state
	signal fsm_n_state : fsm_state;            -- next state 
	---------
	
begin
	
	-- logic of program counter --
	pc: process (CLK, RESET, pc_inc, pc_dec)
	begin
		if RESET = '1' then
			pc_reg <= "000000000000";
		elsif rising_edge(CLK) then
			if pc_inc = '1' then
				pc_reg <= pc_reg + 1;
			elsif pc_dec = '1' then
				pc_reg <= pc_reg - 1;
			end if;
		end if;
	end process;
	-------------------------------
	CODE_ADDR <= pc_reg; -- the value assigning
	
	-- logic of pointer to memory --
	ptr: process (CLK, RESET, ptr_inc, ptr_dec)
	begin
		if RESET = '1' then
			ptr_reg <= "0000000000";
		elsif rising_edge(CLK) then
			if ptr_inc = '1' then
				ptr_reg <= ptr_reg + 1;
			elsif ptr_dec = '1' then
				ptr_reg <= ptr_reg - 1;
			end if;
		end if;
	end process;
	--------------------------------
	DATA_ADDR <= ptr_reg; -- the value assigning
	
	-- logic of bracket counter --
	cnt: process (CLK, RESET, cnt_inc, cnt_dec)
	begin
		if RESET = '1' then
			cnt_reg <= "000000000000";
		elsif rising_edge(CLK) then
			if cnt_inc = '1' then
				cnt_reg <= cnt_reg + 1;
			elsif cnt_dec = '1' then
				cnt_reg <= cnt_reg - 1;
			end if;
		end if;
	end process;
	-------------------------------
	
	-- logic of multiplexer --
	mux: process (CLK, RESET, mux_select)
	begin
		if RESET = '1' then
			mux_output <= "00000000";
		elsif rising_edge(CLK) then
			case mux_select is
				when "00"   => mux_output <= IN_DATA;        -- the value from input 
				when "01"   => mux_output <= DATA_RDATA + 1; -- the incrementing value
				when "10"   => mux_output <= DATA_RDATA - 1; -- the decrementing vlaue
				when "11"   => mux_output <= DATA_RDATA;     -- the value after last reading
				when others => null;
			end case;
		end if;
	end process;
	---------------------------
	DATA_WDATA <= mux_output; -- the value assigning
	
	-- logic of the actual state --
	a_state: process (CLK, RESET, EN)
	begin
		if RESET = '1' then
			fsm_a_state <= s_start;
		elsif rising_edge(CLK) then
			if EN = '1' then
				fsm_a_state <= fsm_n_state;
			end if;
		end if;
	end process;
	-------------------------------
	
	-- logic of the next state --
	n_state: process (fsm_a_state, cnt_reg, IN_VLD, OUT_BUSY, DATA_RDATA, CODE_DATA)
	begin
		pc_inc     <= '0';
		pc_dec     <= '0';
		ptr_inc    <= '0';
		ptr_dec    <= '0';
		cnt_inc    <= '0';
		cnt_dec    <= '0';
		mux_select <= "11";
		DATA_EN    <= '0';
		CODE_EN    <= '0';
		IN_REQ     <= '0';
		OUT_WREN   <= '0';
		DATA_WREN  <= '0';
	
	
		case fsm_a_state is
			-- START --
			when s_start =>
				fsm_n_state <= s_fetch;
			-----------
			
			--	INSTRUCTION READING --
			when s_fetch =>
				CODE_EN <= '1';
				fsm_n_state <= s_decode;
			-------------------------
			
			--	INSTRUCTION DECODING --
			when s_decode => 
				case CODE_DATA is
					when X"3E"  => fsm_n_state <= s_change_ptr;
					when X"3C"  => fsm_n_state <= s_change_ptr;
					
					when X"2B"  => fsm_n_state <= s_data_read;
					when X"2D"  => fsm_n_state <= s_data_read;
					when X"5B"  => fsm_n_state <= s_data_read;
					when X"5D"  => fsm_n_state <= s_data_read;
					when X"2E"  => fsm_n_state <= s_data_read;
					
					when X"2C"  => fsm_n_state <= s_getchar_rq;
					when X"7E"  => fsm_n_state <= s_break_1;
					when X"00"  => fsm_n_state <= s_null;
					when others => fsm_n_state <= s_comments;
				end case;
			---------------------------
			
			-- INC/DEC POINTER --
			when s_change_ptr =>
				case CODE_DATA is
					when X"3E" => ptr_inc <= '1';
					when X"3C" => ptr_dec <= '1';
					when others => null;
				end case;
				
				pc_inc <= '1';
				fsm_n_state <= s_fetch;
			---------------------
			
			-- READ DATA FROM RAM --
			when s_data_read => 
				DATA_EN <= '1';   -- permit data
				DATA_WREN <= '0'; -- reading
				
				case CODE_DATA is
					when X"2B"  =>
						fsm_n_state <= s_change_value;
					when X"2D"  => 
						fsm_n_state <= s_change_value;
					when X"5D"  =>
						fsm_n_state <= s_loop_end_1;
					when X"2E"  =>
						fsm_n_state <= s_print_sym;
					when X"5B"  =>
						pc_inc <= '1';
						fsm_n_state <= s_loop_start_1;
					when others =>
						fsm_n_state <= s_null;
				end case;
			------------------------
			
			-- INC/DEC VALUE AFTER READING --
			when s_change_value =>
				case CODE_DATA is 
					when X"2B" => mux_select <= "01"; -- DATA++
					when X"2D" => mux_select <= "10"; -- DATA--
					when others => null;
				end case;
				
				fsm_n_state <= s_data_write;
			---------------------------------
				
			-- WRITE DATA TO RAM --
			when s_data_write =>
				DATA_EN <= '1';   -- permit data
				DATA_WREN <= '1'; -- writing
				pc_inc <= '1';
				fsm_n_state <= s_fetch;
			-----------------------
		
			-- PRINT THE SYMBOL --			
			when s_print_sym =>
				-- wait if out is busy
				if OUT_BUSY = '1' then
					DATA_EN <= '1';   -- permit data
					DATA_WREN <= '0'; -- reading
					fsm_n_state <= s_print_sym;
				else
					OUT_DATA <= DATA_RDATA; -- permit data
					OUT_WREN <= '1';        -- printing
					pc_inc <= '1';
					fsm_n_state <= s_fetch;
				end if;
			----------------------
			
			-- SCAN THE VALUE --
			when s_getchar_rq =>
				IN_REQ <= '1';      -- request for input
				mux_select <= "00"; -- read input
				fsm_n_state <= s_getchar_scan;
			
			when s_getchar_scan =>
				if IN_VLD /= '1' then
					-- wait input valid
					IN_REQ <= '1';      -- request for input
					mux_select <= "00"; -- read input
					fsm_n_state <= s_getchar_scan;
				else
					DATA_EN <= '1';   -- permit data
					DATA_WREN <= '1'; -- writing
					pc_inc <= '1';
					fsm_n_state <= s_fetch;
				end if;
			--------------------
			
			-- START WHILE LOOP --				
			when s_loop_start_1 =>
				if DATA_RDATA = "00000000" then
					cnt_inc <= '1'; -- +1 '[' symbol => start of the loop
					fsm_n_state <= s_loop_start_2;
				else
					fsm_n_state <= s_fetch;
				end if;
							
			when s_loop_start_2 =>
				if cnt_reg = "000000000000" then
					fsm_n_state <= s_fetch;
				else
					CODE_EN <= '1'; -- read data from ROM
					fsm_n_state <= s_loop_start_3;
				end if;
				
			when s_loop_start_3 =>
				if CODE_DATA = X"5B" then -- '['
					cnt_inc <= '1';
				elsif CODE_DATA = X"5D" then -- ']'
					cnt_dec <= '1';
				end if;
				
				pc_inc <= '1';
				fsm_n_state <= s_loop_start_2;
			----------------------
			
			-- END WHILE LOOP --
			when s_loop_end_1 =>
				if DATA_RDATA = "00000000" then -- number of repetions of the loop
					pc_inc <= '1';
					fsm_n_state <= s_fetch;
				else
					cnt_inc <= '1';
					pc_dec <= '1';
					fsm_n_state <= s_loop_end_2;
				end if;
				
			when s_loop_end_2 =>
				if cnt_reg = "000000000000" then
					fsm_n_state <= s_fetch;
				else
					CODE_EN <= '1'; -- read data from ROM
					fsm_n_state <= s_loop_end_3;
				end if;
				
			when s_loop_end_3 =>
				if CODE_DATA = X"5B" then -- '['
					cnt_dec <= '1';
				elsif CODE_DATA = X"5D" then -- ']'
					cnt_inc <= '1';
				end if;
				
				fsm_n_state <= s_loop_end_4;
			
			when s_loop_end_4 =>
				if cnt_reg = "000000000000" then
					pc_inc <= '1';
				else
					pc_dec <= '1';
				end if;
				
				fsm_n_state <= s_loop_end_2;
			--------------------
			
			-- BREAK FROM LOOP --
			when s_break_1 => 
				pc_inc <= '1'; -- update CODE_ADDR
				cnt_inc <= '1';
				fsm_n_state <= s_break_2;
				
			when s_break_2 =>
				if cnt_reg = "000000000000" then
					fsm_n_state <= s_fetch;
				else
					CODE_EN <= '1'; -- read data from ROM
					fsm_n_state <= s_break_3;
				end if;
				
			when s_break_3 =>
				if CODE_DATA = X"5B" then -- '['
					cnt_inc <= '1';
				elsif CODE_DATA = X"5D" then -- ']'
					cnt_dec <= '1';
				end if;
				
				pc_inc <= '1';
				fsm_n_state <= s_break_2;
			---------------------
		
			-- process other symbols (for example comments) --
			when s_comments =>
				pc_inc <= '1';
				fsm_n_state <= s_fetch;
			--------------------------------------------------
		
			-- null state --
			when s_null =>
				fsm_n_state <= s_null;
			----------------
			
			-- this state cant occur --
			when others => null;
			---------------------------
			
		end case;
	end process;
	-----------------------------
	
end behavioral;
