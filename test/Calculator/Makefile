all: yacc

yacc: Calculator.y
	../../src/ferite-yacc Calculator.y

Calculator.fe: yacc

test: Calculator.fe
	ferite Test.fe

clean:
	/bin/rm -rf *~ Calculator.fe

