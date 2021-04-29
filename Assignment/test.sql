
create table mySets (id integer primary key, iset intSet);

insert into mySets values (1, '{1,2,3}');

insert into mySets values (2, '{1,3,1,3,1}');

insert into mySets values (3, '{3,4,5}');

insert into mySets values (4, '{4,5}');

select * from mySets;

select a.*, b.* from mySets a, mySets b

where (b.iset @> a.iset) and a.id != b.id;

update mySets set iset = iset || '{5,6,7,8}' where id = 4;

select * from mySets where id=4;

select a.*, b.* from mySets a, mySets b

where (b.iset @> a.iset) and a.id != b.id;

select id, iset, (@iset) as card from mySets order by id;

select a.iset, b.iset, a.iset && b.iset
from mySets a, mySets b where a.id < b.id;

delete from mySets where iset @> '{1,2,3,4,5,6}';

select * from mySets;
