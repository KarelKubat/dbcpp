-- Schema definition for the Top2000 database.
-- Some folks insist on typing SQL keywords like TABLE in uppercase
-- but I am too lazy for that.

-- Song titles may occur multiple times in the list. We will only store
-- a title once, even when performed by different artists.
create table title (
  title_id integer primary key autoincrement,
  title text unique
);

-- Artists.
create table artist (
  artist_id integer primary key autoincrement,
  artist text unique
);

-- What artist has which song from which year in the list and at what rating.
create table list (
  title_id  integer references title(title_id),
  artist_id integer references artist(artist_id),
  year      integer,
  rank      integer unique
);  
