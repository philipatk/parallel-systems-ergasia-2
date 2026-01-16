ΟΔΗΓΙΕΣ MAKEFILE

ex1:
    (Το makefile της ασκησης 1 ειναι μεσα στο φακελο "exercise1")
    make
    make clean
    make run (Για αλλαγη παραμετρων απαιτειται αλλαγη στο makefile χειροκινητα)

ex2, ex3:
    (Το makefile των ασκησεων 2,3 ειναι στον εξωτερικο (γενικο) φακελο)
    make EX=2
    make EX=3
    make clean EX=2
    make clean EX=3
    make run EX=2 ARGS="..."
    make run EX=3 ARGS="..."
    παραδειγμα: make run EX=2 ARGS="1000 30 10 4"