#include "DataBaseAccess.h"
sqlite3* DataBaseAccess::_db;
void DataBaseAccess::deleteAlbum(const std::string& albumName, int userId)
{
	std::string sqlStatement =
		"delete from albums "
		"where name = '" + albumName + "' "
		"and USER_ID = " + std::to_string(userId) + "; "
		;

	execute_SQL(sqlStatement, nullptr, nullptr);
}

bool DataBaseAccess::doesAlbumExists(const std::string& albumName, int userId)
{
	std::list<Album> allAlbums;
	std::string sqlStatement = "Select * from albums;";

	execute_SQL(sqlStatement, &allAlbums, callback_GetAlbums);

	for (auto iter : allAlbums)
	{
		if (iter.getName() == albumName && iter.getOwnerId() == userId)
		{
			return true;
		}
	}

	return false;
}

Album DataBaseAccess::openAlbum(const std::string& albumName)
{
	std::list<Album> album;
	std::string sqlStatement = "Select * from albums where name = '" + albumName + "';";

	execute_SQL(sqlStatement, &album, callback_GetAlbums);

	return *album.begin();
}

const std::list<Album> DataBaseAccess::getAlbums()
{
	std::list<Album> albumList;

	std::string sqlStatement = "Select * from Albums;";

	execute_SQL(sqlStatement, &albumList, callback_GetAlbums);
	return albumList;
}

const std::list<Album> DataBaseAccess::getAlbumsOfUser(const User& user)
{
	std::list<Album> albumList;

	std::string sqlStatement = "Select * from Albums where user_id = " + std::to_string(user.getId()) + ';';

	execute_SQL(sqlStatement, &albumList, callback_GetAlbums);
	return albumList;
}

void DataBaseAccess::createAlbum(const Album& album)
{
	std::string sqlStatement =
		"INSERT INTO Albums (NAME, CREATION_DATE, USER_ID) VALUES ('" +
		album.getName() + "', '" +
		album.getCreationDate() + "'," +
		std::to_string(album.getOwnerId()) + ");";


	execute_SQL(sqlStatement, nullptr, nullptr);
}

void DataBaseAccess::printUsers()
{
	const char* sqlStatement = "select * from users;";

	execute_SQL(sqlStatement, nullptr, callback_printTable);
}

void DataBaseAccess::createUser(User& user)
{
	std::string sqlStatement = "insert into users"
		"(name)"
		"values ('" + user.getName() + "');";

	execute_SQL(sqlStatement, nullptr, nullptr);
}

void DataBaseAccess::deleteUser(const User& user)
{
	std::string sqlStatement = "delete from users "
		"where id = " + std::to_string(user.getId()) + ";";

	execute_SQL(sqlStatement, nullptr, nullptr);
}

bool DataBaseAccess::doesUserExists(int userId)
{
	std::list<User> users;
	std::string sqlStatement = "select * from users;";

	execute_SQL(sqlStatement, &users, callback_getUsers);

	for (auto user : users)
	{
		if (user.getId() == userId)
		{
			return true;
		}
	}

	return false;
}

User DataBaseAccess::getUser(int userId)
{
	std::list<User> user;
	std::string sqlStatement = "select * from users where id = " + std::to_string(userId) + ";";

	execute_SQL(sqlStatement, &user, callback_getUsers);

	return *user.begin();
}

int DataBaseAccess::countAlbumsOwnedOfUser(const User& user)
{
	std::string sqlStatement = "select count(*) from albums where user_id = " + std::to_string(user.getId()) + ";";
	std::list<Album> albumList;

	execute_SQL(sqlStatement, &albumList, callback_GetAlbums);

	return albumList.size();
}

int DataBaseAccess::countAlbumsTaggedOfUser(const User& user)
{
	int count = 0;
	bool didFindInAlbum = false;

	const std::list<Album> albumList = getAlbums();

	for (const Album& album : albumList)
	{
		if (!didFindInAlbum)
		{
			for (auto picture : album.getPictures())
			{
				if (!didFindInAlbum) {
					for (auto tag : picture.getUserTags())
					{
						if (tag == user.getId()) {
							count++;
							break;
						}
					}
				}
				else {
					break;
				}
				
			}
		}
		else {
			break;
		}
		
	}

	return count;
}


int DataBaseAccess::countTagsOfUser(const User& user)
{
	int count = 0;
	std::set<int> tags;

	std::string sqlStatement = "select * from tags;";
	execute_SQL(sqlStatement, &tags, callback_GetTags);

	for (auto tag : tags)
	{
		if (tag == user.getId())
		{
			count++;
		}
	}
	return count;
}

float DataBaseAccess::averageTagsPerAlbumOfUser(const User& user)
{
	float count = getAlbums().size(); 

	if (count == 0)
	{
		return 0;
	}
	else {
		return countTagsOfUser(user) / count;
	}
}

User DataBaseAccess::getTopTaggedUser()
{
	std::list<User> topTaggedUser;
	std::string sqlStatement = " SELECT Users.ID, Users.NAME, COUNT(*) AS Tag_Count FROM Users "
		"JOIN ALBUMS on Users.ID = Albums.USER_ID "
		"JOIN Pictures ON ALBUMS.ID = Pictures.ALBUM_ID "
		"JOIN Tags ON Pictures.ID = Tags.PICTURE_ID "
		"GROUP BY Users.ID "
        "ORDER BY Tag_Count DESC LIMIT 1;";

	execute_SQL(sqlStatement, &topTaggedUser, callback_getUsers);

	return *topTaggedUser.begin();
}

Picture DataBaseAccess::getTopTaggedPicture()
{
	std::list<Picture> topTaggedPicture;
	std::string sqlStatement = " SELECT Pictures.*, "
		"COUNT(*) AS Tag_Count "
		"FROM Pictures "
		"JOIN Tags ON pictures.ID = tags.PICTURE_ID "
		"GROUP BY pictures.ID "
		"ORDER BY Tag_Count DESC LIMIT 1;";


	execute_SQL(sqlStatement, &topTaggedPicture, callback_GetPicturesOfAlbum);

	return *topTaggedPicture.begin();
}

std::list<Picture> DataBaseAccess::getTaggedPicturesOfUser(const User& user)
{
	std::list<Picture> taggedPictures;
	std::string sqlStatement = "SELECT PICTURES.* FROM PICTURES "
		"JOIN TAGS ON PICTURES.ID = TAGS.PICTURE_ID "
		"JOIN ALBUMS ON ALBUMS.ID = PICTURES.ALBUM_ID "
		"WHERE TAGS.USER_ID = " + std::to_string(user.getId()) + ";";

	execute_SQL(sqlStatement, &taggedPictures, callback_GetPicturesOfAlbum);

	return taggedPictures;
}


void DataBaseAccess::closeAlbum(Album& pAlbum)
{
	std::string sqlStatement = "delete from albums"
		"where name = '" + pAlbum.getName() + "';";

	execute_SQL(sqlStatement, nullptr, nullptr);
}

void DataBaseAccess::printAlbums()
{
	std::string sqlStatement = "select * from albums;";

	execute_SQL(sqlStatement, nullptr, callback_printTable);
}

void DataBaseAccess::addPictureToAlbumByName(const std::string& albumName, const Picture& picture)
{
	std::list<Picture> PictureList;
	PictureList.push_back(picture);

	insertPictures(PictureList, albumName);
}

void DataBaseAccess::removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName)
{
	std::string sqlStatement = "DELETE FROM pictures "
		"WHERE name = '" + pictureName + "' AND album_id = (SELECT id FROM albums WHERE name = '" + albumName + "');";
	
	execute_SQL(sqlStatement, nullptr, nullptr);
}

void DataBaseAccess::tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	std::string sqlStatement = "INSERT INTO tags (user_id, picture_id) "
		"SELECT " + std::to_string(userId) + ", (SELECT id FROM pictures WHERE name = '" + pictureName + "') ";

	execute_SQL(sqlStatement, nullptr, nullptr);
}

void DataBaseAccess::untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	std::string sqlStatement = "DELETE FROM tags "
		"WHERE picture_id = (SELECT id FROM pictures WHERE name = '" + pictureName + "') "
		"AND user_id = " + std::to_string(userId) + "; ";

	execute_SQL(sqlStatement, nullptr, nullptr);
}

bool DataBaseAccess::open()
{
	std::string dbFileName = "GalleryData.sqlite";
	int file_exist = _access(dbFileName.c_str(), 0);
	int res = sqlite3_open(dbFileName.c_str(), &_db);
	if (res != SQLITE_OK) {
		_db = nullptr;
		std::cout << "Failed to open _db" << std::endl;
		return false;
	}

	execute_SQL("PRAGMA foreign_keys = ON;", nullptr, nullptr);
	if (file_exist != 0) 
	{
		const char* sqlStatement =
			"CREATE TABLE IF NOT EXISTS Users ( \
            ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
            NAME TEXT NOT NULL \
        ); \
        \
        CREATE TABLE IF NOT EXISTS Albums ( \
            ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
            NAME TEXT NOT NULL, \
            CREATION_DATE TEXT NOT NULL, \
            USER_ID INTEGER NOT NULL, \
            FOREIGN KEY (USER_ID) REFERENCES Users(ID) ON DELETE CASCADE \
        ); \
        \
        CREATE TABLE IF NOT EXISTS Pictures ( \
            ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
            NAME TEXT NOT NULL, \
            LOCATION TEXT NOT NULL, \
            CREATION_DATE TEXT NOT NULL, \
            ALBUM_ID INTEGER NOT NULL, \
            FOREIGN KEY (ALBUM_ID) REFERENCES Albums(ID) ON DELETE CASCADE \
        ); \
        \
        CREATE TABLE IF NOT EXISTS Tags ( \
            ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, \
            USER_ID INTEGER NOT NULL, \
            PICTURE_ID INTEGER NOT NULL, \
            FOREIGN KEY (PICTURE_ID) REFERENCES Pictures(ID) ON DELETE CASCADE, \
            FOREIGN KEY (USER_ID) REFERENCES USERS(ID) ON DELETE CASCADE \
        );";


		execute_SQL(sqlStatement, nullptr, nullptr);
	}
	return 0;
}

void DataBaseAccess::close()
{
	sqlite3_close(_db);
	_db = nullptr;
}

void DataBaseAccess::clear()
{
}

int DataBaseAccess::callback_GetAlbums(void* data, int argc, char** argv, char** azColName)
{
	std::list<Album>* AlbumList = static_cast<std::list<Album>*>(data);//converts from a void pointer to a list<album> pointer

	Album newAlbum;
	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "NAME")
		{
			newAlbum.setName(std::string(argv[i]));
		}
		else if (std::string(azColName[i]) == "USER_ID")
		{
			newAlbum.setOwner(std::stoi(argv[i]));
		}
		else if (std::string(azColName[i]) == "CREATION_DATE") {
			newAlbum.setCreationDate(std::string(argv[i]));
		}
	}

	std::list<Picture> albumPictures;
	std::string sqlStatement = "SELECT * FROM PICTURES WHERE ALBUM_ID = (SELECT ID FROM ALBUMS WHERE NAME = '" + newAlbum.getName() + "');";

	execute_SQL(sqlStatement, &albumPictures, callback_GetPicturesOfAlbum);
	for (auto picture : albumPictures)
	{
		newAlbum.addPicture(picture);
	}

	AlbumList->push_back(newAlbum);
	return 0;
}

int DataBaseAccess::callback_GetPicturesOfAlbum(void* data, int argc, char** argv, char** azColName)
{
	std::list<Picture>* PictureList = static_cast<std::list<Picture>*>(data);

	auto newPicture = Picture(0,"");
	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "ID")
		{
			newPicture.setId(std::stoi(argv[i]));
		}
		else if (std::string(azColName[i]) == "NAME")
		{
			newPicture.setName(argv[i]);
		}
		else if (std::string(azColName[i]) == "CREATION_DATE")
		{
			newPicture.setCreationDate(argv[i]);
		}
		else if (std::string(azColName[i]) == "LOCATION")
		{
			newPicture.setPath(argv[i]);
		}
	}
	
	std::string sqlStatement = "Select * from Tags where picture_id = " + std::to_string(newPicture.getId()) + ";";
	std::set<int> TagsList;
	execute_SQL(sqlStatement, &TagsList, callback_GetTags);

	for (auto tag : TagsList)
	{
		newPicture.tagUser(tag);
	}
	
	PictureList->push_back(newPicture);
	return 0;
}

int DataBaseAccess::callback_getUsers(void* data, int argc, char** argv, char** azColName)
{
	std::list<User>* userList = static_cast<std::list<User>*>(data);

	User newUser(0, "");
	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "ID")
		{
			newUser.setId(atoi(argv[i]));
		}
		else if (std::string(azColName[i]) == "NAME") {
			newUser.setName(argv[i]);
		}
	}
	userList->push_back(newUser);
	return 0;
}
int DataBaseAccess::callback_GetTags(void* data, int argc, char** argv, char** azColName)
{
	std::set<int>* TagsList = static_cast<std::set<int>*>(data);

	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "USER_ID")
		{
			TagsList->insert(std::stoi(argv[i]));
		}
	}
	return 0;
}

int DataBaseAccess::callback_printTable(void* data, int argc, char** argv, char** azColName)
{
	for (int i = 0; i < argc; i++)
	{
		std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << ", ";
	}
	std::cout << std::endl;
	return 0;
}

void DataBaseAccess::insertAlbum(Album newAlbum)
{
	std::string sqlStatement = "insert into albums "
		"(NAME, CREATION_DATE, USER_ID) "
		"VALUES('" + newAlbum.getName() + "',' " + newAlbum.getCreationDate() + "', " + std::to_string(newAlbum.getOwnerId()) + ");";

	execute_SQL(sqlStatement, nullptr, nullptr);
}

void DataBaseAccess::insertPictures(std::list<Picture> picturesList, std::string ALbumName)
{
	for (auto picture : picturesList)
	{
		std::string sqlStatement = "insert into pictures "
			"(NAME, LOCATION, CREATION_DATE, ALBUM_ID )"
			" SELECT '" + picture.getName() + "', '" + picture.getPath() + "' , '" + picture.getCreationDate() +
			"', " + "(select id from albums where albums.name = '" + ALbumName + "');"; 
		execute_SQL(sqlStatement, nullptr, nullptr);
	}
}

void DataBaseAccess::insertTags(std::list<Picture> pictureList)
{
	//insertes all of the tags in the picture list's taglists
	for (auto picture : pictureList)
	{
		for (int tag : picture.getUserTags())
		{
			std::string sqlStatement = "insert into TAGS"
				"(PICTURE_ID, USER_ID)"
				"VALUES(" + std::to_string(picture.getId()) + ", " + std::to_string(tag) + ");";

			execute_SQL(sqlStatement, nullptr, nullptr);
		}
	}
}

void DataBaseAccess::execute_SQL(std::string SQL_Statement, void* data, int(*callback)(void*, int, char**, char**))
{
	char* errMessage = nullptr; 
	int res = sqlite3_exec(_db, SQL_Statement.c_str(), callback, data, &errMessage);

	if (res != SQLITE_OK)
	{
		std::string errorMessage = "SQLite error: " + std::string(errMessage);
		sqlite3_free(errMessage);
		throw std::runtime_error(errorMessage);
	}
}
