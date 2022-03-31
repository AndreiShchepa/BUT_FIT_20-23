; run with
;
;   $ z3 -smt2 xshcha00.smt
 
; declare the sorts of pos --- pos represents the chessboard (an 8x8 array) ,
; indexed by "(pos row column)":
; true  --- a queen is on [row, column]
; false --- a queen is not on [row, column]
(declare-fun pos (Int Int) Bool)
 
; a helper function that checks that 1 <= x <= 8
(define-fun is-in-range ((x Int)) Bool (and (> x 0) (<= x 8)))
 
; first, we say that in all rows, there is at least one queen 
(assert (forall ((i Int)) (=> (is-in-range i) (exists ((j Int)) (and (is-in-range j) (pos i j)) ))))
; second, we say that if a queen is on [k; l], then there is no queen on any [m; l] (for m != k) and on any [k; m] (for m != l)
(assert (forall ((k Int)) (forall ((l Int)) (=> (is-in-range k) (=> (pos k l) (forall ((m Int)) (=> (is-in-range m) (and (=> (not (= m k)) (not (pos m l))) (=> (not (= m l)) (not (pos k m))))))) ))))
 
; ADD YOUR CONSTRAINTS HERE
;============================= START ==============

; we have constraints for situations when only one queen can be in each row and each column
; we should write a constraint, that the most one queen can be on any diagonal
; (~X1 | ~X2) & (~X1 | ~X3) & (~X2 | ~X3) & ... & (~X(7) | ~X8)
; for i : (if i z [1,8] & queen is on [i,i]) => (for m : (if m z [1,8] & m != i) => queen is not on [m,m]) - example of first order logic

; i - col
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos i i)) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos m m)))))))

; i + 1 - col 
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos i (+ i 1))) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos m (+ m 1))))))))

; i + 2 - col
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos i (+ i 2))) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos m (+ m 2))))))))

; i + 3 - col
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos i (+ i 3))) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos m (+ m 3))))))))

; i + 4 - col
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos i (+ i 4))) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos m (+ m 4))))))))

; i + 5 - col
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos i (+ i 5))) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos m (+ m 5))))))))

; i + 6 - col
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos i (+ i 6))) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos m (+ m 6))))))))

; i + 7 - col
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos i (+ i 7))) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos m (+ m 7))))))))

; i + 1 - row
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos (+ i 1) i)) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos (+ m 1) m)))))))

; i + 2 - row
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos (+ i 2) i)) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos (+ m 2) m)))))))

; i + 3 - row
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos (+ i 3) i)) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos (+ m 3) m)))))))

; i + 4 - row
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos (+ i 4) i)) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos (+ m 4) m)))))))

; i + 5 - row
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos (+ i 5) i)) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos (+ m 5) m)))))))

; i + 6 - row
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos (+ i 6) i)) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos (+ m 6) m)))))))

; i + 7 - row
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos (+ i 7) i)) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos (+ m 7) m)))))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; another diagonal
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos i (- 9 i))) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos m (- 9 m))))))))

; go down 1
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos i (- 8 i))) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos m (- 8 m))))))))

; go down 2
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos i (- 7 i))) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos m (- 7 m))))))))

; go down 3
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos i (- 6 i))) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos m (- 6 m))))))))

; go down 4
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos i (- 5 i))) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos m (- 5 m))))))))

; go down 5
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos i (- 4 i))) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos m (- 4 m))))))))

; go down 6
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos i (- 3 i))) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos m (- 3 m))))))))

; go down 7
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos i (- 2 i))) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos m (- 2 m))))))))

; go right 1
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos (- 10 i) i)) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos (- 10 m) m)))))))

; go right 2
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos (- 11 i) i)) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos (- 11 m) m)))))))

; go right 3
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos (- 12 i) i)) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos (- 12 m) m)))))))

; go right 4
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos (- 13 i) i)) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos (- 13 m) m)))))))

; go right 5
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos (- 14 i) i)) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos (- 14 m) m)))))))

; go right 6
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos (- 15 i) i)) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos (- 15 m) m)))))))

; go right 7
(assert (forall ((i Int)) (=> (and (is-in-range i) (pos (- 16 i) i)) (forall ((m Int)) (=> (and (is-in-range m) (not (= m i))) (not (pos (- 16 m) m)))))))

;============================= END ================
 
(declare-const y-pos-a Int)
(declare-const y-pos-b Int)
(declare-const y-pos-c Int)
(declare-const y-pos-d Int)
(declare-const y-pos-e Int)
(declare-const y-pos-f Int)
(declare-const y-pos-g Int)
(declare-const y-pos-h Int)
 
(assert (and (is-in-range y-pos-a) (pos 1 y-pos-a)))
(assert (and (is-in-range y-pos-b) (pos 2 y-pos-b)))
(assert (and (is-in-range y-pos-c) (pos 3 y-pos-c)))
(assert (and (is-in-range y-pos-d) (pos 4 y-pos-d)))
(assert (and (is-in-range y-pos-e) (pos 5 y-pos-e)))
(assert (and (is-in-range y-pos-f) (pos 6 y-pos-f)))
(assert (and (is-in-range y-pos-g) (pos 7 y-pos-g)))
(assert (and (is-in-range y-pos-h) (pos 8 y-pos-h)))
 
(check-sat)
(get-model)
