#include <iostream>
#include <sql3cpp.h>

sql3::Connection *c;

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
