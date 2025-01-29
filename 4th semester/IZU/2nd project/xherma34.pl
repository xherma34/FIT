% Zadání è. 9:
% Napište program øešící úkol danı predikátem u9(LIN, VOUT), kde LIN je vstupní 
% èíselnı seznam s nejménì dvìma prvky a VOUT je vıstupní promìnná, ve které
% se vrací absolutní hodnota maximálního rozdílu mezi dvìma sousedními prvky 
% v seznamu LIN. 

% Testovací predikáty:                                          % VOUT
u9_1:- u9([5,27.5,-1,28.3,19,-4],VOUT),write(VOUT).           	% 29.3
u9_2:- u9([1.2,3,5,-7.4],VOUT),write(VOUT).            		% 12.4
u9_3:- u9([1,3,5,7],VOUT),write(VOUT).                		% 2
u9_r:- write('Zadej LIN: '),read(LIN),u9(LIN,VOUT),write(VOUT).

u9(LIN,VOUT):-
    diff(LIN, TMP),
    getmax(TMP, VOUT).
    
diff([_], []).
diff([A, B|T1], [H|T2]) :-
    H is abs(A - B),
    diff([B|T1], T2).

getmax([MAX], MAX).
getmax([H|T], MAX) :-
    getmax(T, X),
    biggernum(H, X, MAX).

biggernum(A, B, A) :-
    A >= B.
biggernum(A, B, B) :-
    A < B.
    

