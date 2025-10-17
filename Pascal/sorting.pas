program skibiditoilet;

const n1 = 10;
n2 = 20;
n3 = 50;
n4 = 100;

type mass = array[1..n4] of integer;


var cmp, swp: integer; // Счётчики сравнений и перестановок
x : mass;
swpm, cmpm: array[1..5] of integer; // Для вывода результатов swp и cmp

procedure printMass(var x:mass; n:integer);
var i:integer;
begin
write('Mass : ');
for i:=1 to n do
	write(x[i],' ');
writeln;
writeln('--------------------------------------------------------------------');
end;


procedure createMass(var x: mass; n, code:integer);
var i:integer;
begin

if code = 1 then 
	for i := 1 to n do
		x[i] := i+2
else if code = 2 then
	for i := 1 to n do
		x[i] := 2*n - 2*i
else if code = 3 then
	for i := 1 to n do
		if i mod 2 = 1 then
			x[i] := i + 2
		else x[i] := 2*n - 2*i
else
	for i := 1 to n do
		x[i] := random(2*n) - i;
end;
		

procedure Sort1(var x: mass; n: integer);
var i, j, temp, key: integer;
begin
for i:= 2 to n do
    begin
    key:= x[i];
    j:= i;
    while (j > 1) and (x[j - 1] > key) do
        begin
        temp:= x[j];
        x[j]:= x[j - 1];
        x[j - 1]:= temp;
        j:= j - 1;
        swp:= swp + 1;
	 end;
	cmp:=swp+n-1;
    x[j]:= key;
 end;
end;






procedure Sort2(var x:mass; n:integer);
var y: mass;

procedure merge_sort(var x,y:mass; a,b : integer);
var c, t1, t2, i: integer;
begin
cmp := cmp + 1;
if b > a then
	begin
	c := (a+b) div 2;
	merge_sort(x, y, a, c);
	merge_sort(x, y, c+1, b); // Рекурсивно сортируем
	t1 := a; // Начало лев отсорт. отрезка
	t2 := c+1; // Начало прав отсорт. отрезка
	
	for i := a to b do 
		begin
		if (t1 <= c) and (t2 <= b) then
			if (x[t1] < x[t2]) then
				begin
				y[i] := x[t1];
				t1 := t1 + 1;
				end
			else 
				begin
				y[i] := x[t2]; 
				t2 := t2 + 1;
				end
		else 
			if t2 > b then
				begin
				y[i] := x[t1];
				t1 := t1 + 1;
				end
			else 
				begin
				y[i] := x[t2]; 
				t2 := t2 + 1;
				end;
		swp := swp + 1;
		cmp := cmp + 2;
		end;
		
	for i := a to b do
		begin
		x[i] := y[i];
		swp := swp + 1;
		end;
	end; 
end;

begin
cmp := 0;
swp := 0;
merge_sort(x, y, 1, n);
end;


procedure Body(code : integer);
var j, n: integer;
s1, s2: real;
begin
if code = 1 then writeln('Selection sort')
else writeln('Merge sort');
writeln(' n | argument |           sequence      number           | average |');
writeln('   |          |       1      2      3      4      5      |         |');
writeln('--------------------------------------------------------------------');
n := 0;
while n <> n4 do
	begin
	if n = 0 then n := n1
	else if n = n1 then n := n2
	else if n = n2 then n := n3
	else n := n4;
	s1 := 0;
	s2 := 0;
	writeln(n:3,'|          |                                          |         |');
	writeln('--------------------------------------------------------------------');
	for j := 1 to 5 do
		begin
		createMass(x, n, j);
		write('Unsorted! ');
		printMass(x, n);
		if code = 1 then Sort1(x,n)
		else Sort2(x,n);
		cmpm[j] := cmp;
		swpm[j] := swp;
		write('Sorted! ');
		printMass(x, n);
		writeln(j, ': Comparing: ',cmp, ' Swapping: ',swp);
		writeln('--------------------------------------------------------------------');
		end;
	write('   |comparing |  ');
	for j := 1 to 5 do
		begin
		write(cmpm[j]:6,' ');
		s1 := s1 + cmpm[j];
		end;

	writeln('     | ', (s1 / 5):7:3,' |');
	write('   |swapping; |  ');
	for j := 1 to 5 do
		begin
		write(swpm[j]:6,' ');
		s2 := s2 + swpm[j];
		end;
	writeln('     | ', (s2 / 5):7:3,' |');
	writeln('--------------------------------------------------------------------');
	end;
end;
		

begin
randomize;
Body(1);
writeln;
writeln;
writeln;
Body(2);
end.

