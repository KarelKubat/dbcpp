# The Dutch Top2000 Tracks (voted in 2016)

The Netherlands has an end-of-year tradition where people vote their favorite
tracks, which are then played on national radio. Traditionally the top 1 track
is played just before midnight, December 31st; just before the fireworks start.

Let's make a program `top2000` that can store tracks in a database and that
shows us some interesting features of the list, e.g.:

*  What is the top 10 (obviously)
*  Which are the best 10 artists; "best" being arbitrarily defined as having
   the most entries in the list
*  Which titles occur most frequently in the list.

We could extend this list if "interesting" features ad-hoc; e.g., by analyzing
whether tracks from certain year(s) or decade(s) are most favored by the
listeners, but that's left to the reader.

## Files and Organization

### Input Data

The raw data is [top2000list.txt](top2000list.txt), a textfile where each line
has four tab-separated items:

1.  The rank in the list
1.  The track title
1.  The artist
1.  The year that the track appeared.

Lines from this file are picked up by a trivial Perl script
[loadlist.pl](loadlist.pl) and fed to `top2000` for addition to the database.

### C++ Program

The C++ program is coded in one source file [top2000.cc](top2000.cc) which
is compiled into an executable program `top2000`. The parts of this program,
and what they are responsible for, is explained below.

### Trying it Yourself

The [Makefile](Makefile) binds it all together. It can be invoked as
follows:

*  `make`: Creates the database `top2000.db` and loads table definitions from
   `top2000.sql`. Builds the program `top2000` and starts `loadlist.pl` which in
   turn is responsible for calling the program `top2000` to add entries. Finally
   calls `top2000 analyze` to produce an analysis.
*  `make clean`: Removes all artifacts.

Before running `make` in this directory, make sure that you run `make` in the
top level directory to create all necessary libraries. You will need to have
the development environment for Sqlite3 installed, which is done using e.g.
`apt-get install libsqlite3-dev` on Debian-based systems.

## Database Schema

The database schema to represent the top2000 list is relatively straight
forward and kept in a separate file [top2000.sql](top2000.sql):

*  Table `title` holds separate titles,
*  Table `artist` holds separate artists,
*  Table `list` is a cross-reference table that binds the above two
   together using foreign keys, and adds the rank in the list and year of
   appearance:

```sql
create table title (
  title_id integer primary key autoincrement,
  title text unique
);

create table artist (
  artist_id integer primary key autoincrement,
  artist text unique
);

create table list (
  title_id  integer references title(title_id),
  artist_id integer references artist(artist_id),
  year      integer,
  rank      integer unique
);
```

The procedure to add a new entry of a title, artist, rank and year will consist
of checking whether the title already is present (if not, it must be added);
whether the artist's name is already present (if not, it must be added), and
then adding the information into table `list` (unless it isn't present already,
we will allow invocations to add the same data without issuing an error).

## Program Parts

### Program start: main()

The program `top2000` can be called in two ways which is reflected in
`main()`:

*  `top2000 add RANK TITLE ARTIST YEAR` to add an entry to the database, or
*  `top2000 analyze` to analyze the data.

Also, `main()` initializes a global variable `sql3::Connection *c`, the database
connection which is reused in other parts of the program. Alternatively this
could be a parameter to other functions.

```c++
sql3::Connection *c;

int main(int argc, char **argv) {
  try {
    // Connect to the db.
    c = new sql3::Connection("top2000.db");

    // Check args and act
    if (argc == 6 && std::string(argv[1]) == "add")
      add(atoi(argv[2]), argv[3], argv[4], atoi(argv[5]));
    else if (argc == 2 && std::string(argv[1]) == "analyze")
      analyze();
    else
      throw
	"Usage: top2000 add RANK TITLE ARTIST YEAR\n"
	"   or: top2000 analyze\n";
  } catch (std::exception const &e) {
    std::cerr << e.what() << '\n';
    return 1;
  } catch (std::string const &s) {
    std::cerr << s << '\n';
    return 1;
  } catch (char const *s) {
    std::cerr << s << '\n';
    return 1;
  } catch (...) {
    std::cerr << "Unknown exception caught\n";
    return 1;
  }

  return 0;
}
```

### Adding an entry: function add()

Adding an entry consists of:

*  Inserting a title and remembering its `title_id`, or fetching the ID
   of a previously inserted title;
*  Inserting an artist name and remembering its `artist_id`, or fetching
   the ID of a previously inserted artist;
*  Inserting the combination of the `title_id`, `artist_id`, rank and year
   into table `list` if it is not there yet.

This is a fairly straight-forward function. The code illustrates how:

*  An `sql3::Transaction` object is constructed, using the global database
   connection `sql3::Connection`
*  How statements are prepared
*  How variables are bound
*  How prepared and bound statements are executed
*  How results are fetched from `select` statements.

```c++
void add (int rank, std::string title, std::string artist, int year) {
  sql3::Transaction t(c);

  std::cout << "Rank " << rank << ": " << title
	    << " (" << artist << ", " << year << ")";

  // Look up or insert the title.
  t.prepare("select title_id from title where title = ?")
    .bind(title)
    .execute();
  int title_id = t.colint();
  if (title_id == 0) {
    t.prepare("insert into title (title) values (?)")
      .bind(title)
      .execute();
    title_id = t.lastid();
  }

  // Lookup or insert the artist.
  t.prepare("select artist_id from artist where artist = ?")
    .bind(artist)
    .execute();
  int artist_id = t.colint();
  if (artist_id == 0) {
    t.prepare("insert into artist (artist) values (?)")
      .bind(artist)
      .execute();
    artist_id = t.lastid();
  }

  // Insert in the list if it isn't here yet.
  t.prepare("select count(*) from list "
	    "where title_id=? and artist_id=? and year=? and rank=?")
    .bind(title_id).bind(artist_id).bind(year).bind(rank).execute();
  if (t.colint() != 0) {
    std::cout << "  ... already present\n";
    return;
  }
  t.prepare("insert into list (title_id, artist_id, year, rank) "
	    "values (?,?,?,?)")
    .bind(title_id).bind(artist_id).bind(year).bind(rank).execute();
  std::cout << "  ... inserted\n";
}
```

### Analysis of the data: function analyze()

Below is the listing of function `analyze()`. The SQL queries that are used are
not further explained here (see a guide on relational databases for that).  The
code is meant as an example on how to interface with a Sqlite3 database.  Again,
as in `add()`, a transaction object is constructed, statements are prepared,
variables are bound, prepared statements are executed, and results from
`select` statements are fetched.

```c++
void analyze() {
  sql3::Transaction t(c);

  // Display the top 10.
  std::cout << "\nTop 10 tracks:\n";
  t.prepare("select   l.rank, l.year, a.artist, t.title "
	    "from     list l, artist a, title t "
	    "where    l.rank <= 10 "
	    "and      l.artist_id = a.artist_id "
	    "and      l.title_id = t.title_id "
	    "order by l.rank");
  while (t.execute())
    std::cout << "At rank " << t.colint(0) << ": " << t.colstr(3)
	      << " (" << t.colstr(2) << ", " << t.colint(1) << ")\n";

  // Display the 10 most frequently present artists.
  std::cout << "\nTop 10 artists:\n";
  t.prepare("select a.artist, count(l.artist_id) as count "
	    "from     list l, artist a "
	    "where    l.artist_id = a.artist_id "
	    "group by l.artist_id "
	    "order by count desc "
	    "limit    10");
  while (t.execute())
    std::cout << "Occurring " << t.colint(1) << " times: "
	      << t.colstr(0) << '\n';

  // Which titles occur more than 2 times?
  std::cout << "\nReused titles or covers (3 times or more often)\n";
  t.prepare("select   t.title, count(l.title_id) as times "
	    "from     title t, list l "
	    "where    t.title_id = l.title_id "
	    "group by t.title_id "
	    "order by times desc");
  while (t.execute()) {
    if (t.colint(1) < 3)
      break;
    std::cout << t.colint(1) << " times: " << t.colstr(0) << '\n';

    // Fetch who performed that title, when, and the ranking.
    // This needs its own transaction, since t is still running.
    sql3::Transaction u(c);
    u.prepare("select   l.year, l.rank, a.artist "
	      "from     list l, artist a, title t "
	      "where    l.artist_id = a.artist_id "
	      "and      l.title_id = t.title_id "
	      "and      t.title = ? "
	      "order by l.rank").bind(t.colstr(0));
    while (u.execute())
      std::cout << "  performed by " << u.colstr(2)
		<< " (year " << u.colint(0) << ", rank " << u.colint(1) << '\n';
  }
}
```

## Sample output

And finally, this is what the analysis should produce:

```
Top 10 tracks:
At rank 1: Bohemian Rhapsody (Queen, 1975)
At rank 2: Hotel California (Eagles, 1977)
At rank 3: Stairway To Heaven (Led Zeppelin, 1971)
At rank 4: Piano Man (Billy Joel, 1974)
At rank 5: Child In Time (Deep Purple, 1972)
At rank 6: Avond (Boudewijn de Groot, 1997)
At rank 7: Heroes (David Bowie, 1977)
At rank 8: Mag Ik Dan Bij Jou (Claudia de Breij, 2011)
At rank 9: Wish You Were Here (Pink Floyd, 1975)
At rank 10: Black (Pearl Jam, 1991)

Top 10 artists:
Occurring 38 times: The Beatles
Occurring 26 times: The Rolling Stones
Occurring 25 times: Queen
Occurring 23 times: Coldplay
Occurring 23 times: U2
Occurring 22 times: David Bowie
Occurring 22 times: Michael Jackson
Occurring 21 times: Bruce Springsteen
Occurring 21 times: ABBA
Occurring 16 times: Bløf

Reused titles or covers (3 times or more often)
4 times: Crazy
  performed by Seal (year 1991, rank 915
  performed by Gnarls Barkley (year 2006, rank 1662
  performed by Aerosmith (year 1993, rank 1835
  performed by Icehouse (year 1987, rank 1878
3 times: Home
  performed by Dotan (year 2014, rank 82
  performed by Michael Bublé (year 2004, rank 1382
  performed by Simply Red (year 2003, rank 1789
3 times: Somebody To Love
  performed by Queen (year 1976, rank 58
  performed by George Michael (year 1993, rank 1054
  performed by Jefferson Airplane (year 1970, rank 1512
3 times: Sorry
  performed by Kyteman (year 2009, rank 132
  performed by Kensington (year 2016, rank 365
  performed by Justin Bieber (year 2015, rank 874
3 times: Hallelujah
```
