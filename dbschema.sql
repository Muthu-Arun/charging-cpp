CREATE TABLE outlet(id integer primary key autoincrement, name varchar, station_id integer, status varchar, foreign key(station_id) references station(id));
CREATE TABLE user(id integer primary key autoincrement, name varchar, email varchar, password varchar);
CREATE TABLE station(id integer primary key autoincrement, name varchar, location varchar);
CREATE TABLE session(id integer primary key autoincrement, user_id integer, outlet_id integer, session_state varchar);