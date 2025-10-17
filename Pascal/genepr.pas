uses Crt;
 
const
  n = 10;
 
type
  mas = array [1..n] of word;
  arr = array [1..n] of double;
 
var
  t: text;
  pop: mas; {Population}
  a: arr; {Chances to mix}
  iter, bestRoot, count, num1, num2, exitNumber, i, number, mode, output, leave: word;
  errorIndex: integer;
  chance: double; {Chance to mutate}
  input: string; {Answer of user}
 
function F(x: double): double;
begin
  F := x*(x-2)*(x-2.75)*(exp(x/10))*(cos(x/10))*(2-(exp((x-2)*ln(3))));
end;
 
{Creating the first population}
procedure Init(var pop: mas; n: integer);
var
  i: integer;
begin
  for i := 1 to n do pop[i] := Random(65533) + 1;
end;
 
{Sorting the population accroding to values of function in those points}
procedure Sorting(var a: arr; var pop: mas);
var
  i, j, b: word;
  c: double;
begin
  for j := 1 to n - 1 do
    for i := 1 to n - j do
      if F(pop[i] / 16384) > F(pop[i + 1] / 16384) then begin
        c := a[i]; b := pop[i];
        a[i] := a[i + 1]; pop[i] := pop[i + 1];
        a[i + 1] := c; pop[i + 1] := b;
      end;
end;
 
{Random chance}
function Ver(n: integer): double;
begin
  ver := (Random(n) + 1) / 100;
end;
 
{Mixing two persons}
procedure Skr(var kor1, kor2: word; var ver1, ver2: double);
var
  pos1, pos2, i, st: word;
begin
  if Ver(100) <= Frac(ver1) then ver1 := Trunc(ver1) + 1
  else ver1 := Trunc(ver1);
  if Ver(100) <= Frac(ver2) then ver2 := Trunc(ver2) + 1
  else ver2 := Trunc(ver2);
  if (ver1 >= 1 ) or (ver2 >= 1 ) then begin
      {Process of mixing}
    pos1 := Random(15) + 1;
    Repeat
      pos2 := Random(15) + 1;
    Until pos2 <> pos1;
    if pos1 > pos2 then begin
      i := pos1;
      pos1 := pos2;
      pos2 := i;
    end;
      {Changing of 0 and 1}
    for i := pos1 to pos2 do
    begin
      st := Round(Exp((16 - i) * Ln(2)));
      if ((kor1 and st) <> 0 ) and ((kor2 and st) = 0) then begin
        kor1 := kor1 - st;
        kor2 := kor2 + st;
      end
      else if ((kor1 and st) = 0) and ((kor2 and st) <> 0) then begin
        kor1 := kor1 + st;
        kor2 := kor2 - st;
      end;
    end;
    ver1 := ver1 - 1;
    ver2 := ver2 - 1;
  end;
end;
 
procedure Mutation(var person: word; chance: double);
var
  j, k, first, start: word;
begin
  first := person;
  start := Random(15) + 1;
  if Ver(100) < chance then begin
    j := start;
    k := 16;
    while j < k do
    begin
      {Changing of 0 and 1}
      if (person and Round(Exp((16 - j) * Ln(2))) <> 0) and (person and Round(Exp((16 - k) * Ln(2))) = 0) then
        person := person - Round(Exp((16 - j) * Ln(2))) + Round(Exp((16 - k) * Ln(2)))
      else if (person and Round(Exp((16 - j) * Ln(2))) = 0) and (person and Round(Exp((16 - k) * Ln(2))) <> 0) then
        person := person + Round(Exp((16 - j) * Ln(2))) - Round(Exp((16 - k) * Ln(2)));
      j := j + 1;
      k := k - 1;
    end;
  end;
  {If during the mutation the value became worse, do the mutation again}
  if F(first / 16384) > F(person / 16384) then Mutation(first, chance);
end;
 
procedure Selection(var pop: mas);
var
  i: integer;
  sum, midval: double;
begin
  sum := 0;
  for i := 1 to n do
  begin
    a[i] := F(pop[i] / 16384);
    sum := sum + a[i];
  end;
  midval := sum / n;
  for i := 1 to n do a[i] := a[i] / midval;
  Sorting(a, pop);
  for i := 1 to n do if a[i] < 0 then pop[i] := Random(65534) + 1;
end;
 
function IntToBin(x: word):string;
var
r,c:string;
begin
  r:='';
  if x<0 then c:='-' else c:='';
  Repeat
    r:=chr(ord('0')+(x mod 2))+r;
    x:=x div 2;
  Until x=0;
  r:=c+r;
  While Length(r) <> 16 do r:= '0' + r;
  IntToBin:=r;
end;
 
begin
Repeat
  ClrScr;
  Assign(t, 'log.txt' );
  Rewrite(t);
  Randomize;
  bestRoot := 0;
  Init(pop, n);
  Write('Enter the chance of mutation: ');
  Repeat
    Readln(input);
    Val(input, chance, errorIndex);
    if errorIndex <> 0 then Write('Incorrect value of chance. Please, re-enter: ');
  Until errorIndex = 0;
  iter := 0;
  Write('Enter the number of mixing persons: ');
  Repeat
    Readln(input);
    Val(input, number, errorIndex);
    if errorIndex <> 0 then Write('Incorrect number of mixing persons. Please, re-enter: ');
  Until errorIndex = 0;
  Write('Enter the number of iterations without changing the result: ');
  Repeat
    Readln(input);
    Val(input, exitNumber, errorIndex);
    if errorIndex <> 0 then Write('Incorrect number of iterations. Please, re-enter: ');
  Until errorIndex = 0;
  Write('Test (1) or basic (2) mode? : ');
  Repeat
    Readln(input);
    Val(input, mode, errorIndex);
    if ( errorIndex <> 0 ) and ( mode <> 1 ) and ( mode <> 2) then Write('Incorrect mode code. Please, re-enter: ');
  Until errorIndex = 0;
  if mode = 1 then begin
  Write('Output to the screen (1) or to the file (2)? : ');
  Repeat
    Readln(input);
    Val(input, output, errorIndex);
    if ( errorIndex <> 0 ) and ( output <> 1 ) and ( output <> 2) then Write('Incorrect output code. Please, re-enter: ');
  Until errorIndex = 0;
  end;
  Repeat
    if output = 1 then begin
      Writeln('Population ', iter + 1);
      for i := 1 to n do Writeln( inttobin(pop[i]),' ', pop[i]/16384, ' ', F(pop[i]/16384));
    end else if output = 2 then begin
      Writeln(t, 'Population ', iter + 1);
      for i := 1 to n do Writeln(t, inttobin(pop[i]),' ', pop[i]/16384, ' ', F(pop[i]/16384));
    end;
    iter := iter + 1;
    Selection(pop);
    Sorting(a, pop);
    bestRoot := pop[n];
    for i := 1 to number do
    begin
      num1 := Random(n) + 1;
      Repeat num2 := Random(n) + 1; Until num1 <> num2;
      if (a[num1] > 0 ) and (a[num2] > 0 ) then Skr(pop[num1], pop[num2], a[num1], a[num2]);
    end;
    for i := 1 to n do Mutation(pop[i], chance);
    Sorting(a, pop);
    pop[1] := bestRoot;
    {Including the best value from the previous population to the current}
    Sorting(a, pop);
    if Abs(F(bestRoot / 16384) - F(pop[n] / 16384)) < 1e-5 then count := count + 1
    else count := 1;
  Until count = exitNumber;
  Writeln;
  if ( output = 1 ) or ( mode = 2 ) then begin Writeln('Max value  = ', F(pop[n] / 16384), ' at x = ', pop[n] / 16384, ' calculated in ', iter, ' iterations'); end
  else if output = 2 then Writeln(t, 'Max value  = ', F(pop[n] / 16384), ' at x = ', pop[n] / 16384, ' calculated in ', iter, ' iterations');
  Close(t);
  Write('Do you want to continue? (1 - yes, 2 - no) : ');
  Repeat
    Readln(input);
    Val(input, leave, errorIndex);
    if ( errorIndex <> 0 ) and ( leave <> 1 ) and ( leave <> 2) then Write('Incorrect leave code. Please, re-enter: ');
  Until ( errorIndex = 0 ) and ( ( leave = 1) or ( leave = 2 ) );
Until leave = 2;
end.
