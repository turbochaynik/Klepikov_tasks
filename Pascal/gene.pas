program generation;
const N=10;
type
mas = array[1..N] of int64;

var pop,das: mas; k1,k2,i,j,t,k,o,l:int64; maxx,maxf:double;
fil:text; name:string;

//функция, у которой находится максимум
function F(x:double):double;
begin
	F:=(x-3)*(x-2)*exp(x-0.01)*4*exp(x-3.99)*4*(1-exp(x-1.5))*sin(x/3 + 0.2);
end;

//первая популяция
procedure firgen(var pop: mas);
var q:int64;
begin
	for q:=1 to N do 
	pop[q]:=Random(16384*4 - 1) + 1;
end;

//сортировка по значению функции
procedure sort(var pop:mas);
var q,y,a:int64; 
begin
for y:=1 to N-1 do
	for q:=1 to N do
		begin
			if F(pop[q]/16384) < F(pop[q+1]/16384) then begin
				a:=pop[q]; pop[q]:=pop[q+1]; pop[q+1]:=a; end;
		end;
end;

//нахождения родителей и создание новой особи
procedure papa(pop:mas);			
var S,a,p,m:double; k:integer;
begin
S:=0; a:=0;
for k:=1 to N do S:=S+F(pop[k]/16384);
p:=(Random((trunc(S))*16384-1)/16384);
m:=(Random((trunc(S))*16384-1)/16384);
for k:=1 to N do
	if ((p>=a) and (p<(a+F(pop[k]/16384)))) then begin
		k1:=(pop[k] - (pop[k] mod 128)); k2:= (pop[k] mod 128); break; end
	else begin a:=a+F(pop[k]/16384); end;
a:=0;
for k:=1 to N do
        if ((m>=a) and (m<(a+F(pop[k]/16384)))) then begin
                k1:=k1+(pop[k] mod 128); k2:=k2+pop[k]-(pop[k] mod 128); break; end
        else begin a:=a+F(pop[k]/16384); end;
end;

//мутация
function mutant(d:int64):int64;
begin
d:=abs(d-random(16384));
mutant:=d;
end;



//сама программа
begin
maxf:=0;
k:=0;
firgen(pop);
writeln('1 - основной режим, 2 - тестовый'); readln(k); if k = 2 then begin writeln('1 - на экран, 2 - в файл'); readln(l); end else l:=0;
if l=2 then begin
	writeln('название файла');
        read(name);
        assign(fil,name);
        rewrite(fil);
end;

for i:=1 to 10000 do
	begin
	k1:=0; k2:=0;
	sort(pop);
	das:=pop;
	if maxf<(F(pop[1]/16384)-1) then begin maxx:=pop[1]/16384; maxf:=F(pop[1]/16384)-1; k:=i; end;
	for j:=1 to 5 do begin  papa(pop);
	das[2*j-1]:=k1;  das[2*j]:=k2;
		end;
	for j:=1 to 10 do
		begin
		o:=Random(99)+1;
		if o<=15 then das[j]:=mutant(das[j]);
		end;	
	pop:=das;
	if l=1 then begin
		writeln('GENERATION ', i);
       	 	for t:=1 to 10 do begin writeln('точка x - ', pop[t]/16384, '    значение ф-и - ', F(pop[t]/16384)-1); end; writeln;
	end else if l=2 then begin
		 writeln(fil,'GENERATION ', i);
        for t:=1 to 10 do begin writeln(fil, 'точка x - ', pop[t]/16384, '    значение ф-и - ', F(pop[t]/16384)-1); end; writeln(fil, ' ');
        end;
	end;
if l=2 then close(fil);

sort(pop);
if l=0 then
writeln('точка максимума - ', maxx, '     значение ф-и -  ', maxf, '      кол-во итераций ', k); 
end.
