{641}
program integral;
const e = 0.001;
e1 = e/1000; e2 = e/1000;
type tf = function (x:real):real;
var x1, x2, x3, s1, s2, s3: real;

procedure root(f, g, df, dg : tf; a, b: real; var x: real);
var c1, c2 : real; flag : boolean = false; k:integer;
begin
k:=1;
if f(a) - g(a) < 0 then 
	if f((a+b)/2) - g((a+b)/2) < (f(a)-g(a) + f(b)-g(b))/2 then flag := true
else if f(a) - g(a) > 0 then
	if f((a+b)/2) - g((a+b)/2) > (f(a)-g(a) + f(b)-g(b))/2 then flag := true;
if flag then while true do
      begin
      c1 := b - (f(b)-g(b))/(df(b)-dg(b));
      c2 := (a*(f(b)-g(b))-b*(f(a)-g(a)))/(f(b)-g(b)- (f(a)-g(a)));
      if (abs(b-a)>e1) then begin b := c1; a :=c2; k:=k+1; end
      else begin x := (2*c1-e1)/2; writeln(k); break end
      end
else while true do
      begin
      c1 := a - (f(a)-g(a))/(df(a)-dg(a));
      c2 := (a*(f(b)-g(b))-b*(f(a)-g(a)))/(f(b)-g(b)- (f(a)-g(a)));
      if (abs(b-a)>e1) then begin a := c1; b := c2; k:=k+1; end
      else begin x := (2*c2+e1)/2; writeln(k); break end
      end
end;

function integral(f : tf; a, b:real):real;
var h, sum1, sum2: real; n: integer = 20; i,k : integer;
begin
k:=1;
h := (b-a)/n; sum2 := 0; sum1 := 0;
for i := 0 to n-1 do
	sum1 := sum1 + f(a+(i+0.5)*h);
sum1 := h*sum1;
while true do
begin
	n := n*2; h := (b-a)/n; sum2 := 0;
	for i := 0 to n-1 do
      		sum2 := sum2 + f(a+(i+0.5)*h);
    	sum2 := h*sum2;
    	if abs(sum1-sum2)/3 < e2 then begin 
		integral := sum2; writeln(k); break end
    	else begin sum1 := sum2; k:=k+1; end
  	end
end;
   
function f1(x:real): real; begin f1:= 0.6*x+3 end;
function f2(x:real): real; begin f2:= (x-2)*(x-2)*(x-2)-1 end;
function f3(x:real): real; begin f3:= 3/x end;
function df1(x:real): real;begin df1:= 0.6 +x*0 end;
function df2(x:real): real;begin df2:= 3*x*x-12*x+12 end;
function df3(x:real): real;begin df3:= -3/(x*x) end;
        
begin
x1:=0; x2:=0; x3:=0;
root(@f2, @f1, @df2, @df1, 3, 6, x1);
writeln('x1 = ', x1);
root(@f3, @f2, @df3, @df2, 0.5, 6, x2);
writeln('x2 = ', x2);
root(@f3, @f1, @df3, @df1, 0.1, 1, x3);
writeln('x3 = ' , x3);
writeln;
s1:= integral(@f1, x3, x1);
s2:= integral(@f2, x2, x1);
s3:= integral(@f3, x3, x2);
writeln;
writeln('s1 = ', s1:10:6); writeln('s2 = ', s2:10:6); writeln('s3 = ', s3:9:6); 
writeln;
writeln('ANSWER: ', s1-s2-s3:9:6)
end.
