program mypr;
var wit,witout,a,b, k:integer; zf, sf, cf, ofl: boolean;

procedure summator(a, b:integer);
begin
if a+b > 128 then wit:= a+b-256 else
if a+b < -128 then wit:= a+b+256 else
wit:= a+b;
if (wit>0) and (wit < 128) then witout:=wit else
witout:= (256 - abs(wit)) mod 256;

if wit = 0 then zf := true else zf:=false;
if wit<0 then sf:=true else sf:=false;
if (abs(a+b)>255) or (a+b<0) then cf:=true else cf:=false;
if (a>0) and (b>0) and (wit <0) then ofl:=true else
if (a<0) and (b<0) and (wit >0) then ofl:=true else ofl:=false;
end;

procedure razn(a, b:integer);
begin
if abs(a-b) < 129 then wit:=a-b else wit:= abs(a-b) - 256;
if (wit>0) and (wit<128) then witout:=wit else witout:=(256-abs(wit)) mod 256;

if wit = 0 then zf := true else zf:=false;
if wit<0 then sf:=true else sf:=false;
if a<0 then a:=256+a; if b<0 then b:=256+b;
//if (a>0) and (wit<0) then cf:=true else if (a<0) and (wit>0) then cf:=true else cf:=false;
if ((a>0) and (b>0) and (wit<0) ) or ((a<0) and (b<0) and (wit>0)) then cf:=true else cf:=false;
if a>=128 then a:=a-256;
if b>=128 then b:=b-256;
if ((a>0) and (b<0) and (wit<0)) or ((a<0) and (b>0) and (wit>0)) then ofl:=true else ofl:=false;
end;

begin
readln(a);
readln(b);
read(k); if k = 1 then
summator(a, b) else razn(a,b);
writeln('zf = ', zf);
writeln('sf = ', sf);
writeln('cf = ', cf);
writeln('ofl = ', ofl);
writeln(wit, '   ', witout);
end.
