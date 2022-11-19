#include <iostream>
#include <string>
using namespace std;

/*
*	Комментарий от автора:
*	
*	Это моя первая программа на C++, и я не знаю, как адекватно делаются
*	композиция с агрегацией, поэтому тут нарушены как связи между объектами,
*	так и инкапсуляция.
* 
*	Надеюсь вы меня поймёте :P
*/

class Tile { // Класс, отвечающий за определённую клетку
	char symbol;
	bool hasCollision = false; // Имеет ли он коллизию
	bool moveable = false; // Может быть он передвигаемым
public:
	Tile(char symbol, bool hasCollision = false) {
		this->symbol = symbol;
		this->hasCollision = hasCollision;
	}

	char GetSymbol() {
		return symbol;
	}

	bool GetHasCollision() {
		return hasCollision;
	}

	bool GetMoveable() {
		return moveable;
	}
};

class Entity : public Tile { // Класс сущности (Может двигаться)
public:
	struct Position {
		int x;
		int y;
	};

	Entity(char symbol, int x, int y) : Tile(symbol, false) {
		this->position.x = x;
		this->position.y = y;
	};

	Position GetPosition() {
		return position;
	}
private:
	Position position;
	bool moveable = true;
};

class Player : public Entity { // Класс игрока (как сущность, но его ссылка сохраняется в отдельном поле)
public:
	Player(int x, int y) : Entity('@', x, y) {};
};

class Map { // Класс карты
	Tile** tiles;
	Player* player;
	int maxX;
	int maxY;
	int size;
public:
	Map(int maxX, int maxY, int playerX = 1, int playerY = 1) {
		this->maxX = maxX;
		this->maxY = maxY;
		size = maxX * maxY;
		tiles = new Tile*[size];

		for (int i = 0; i < size; i++) {
			tiles[i] = new Tile(',');
		}

		GeneratePlayer(playerX, playerY);
	}

	Map(int maxX, int maxY, char collisionable[], char map[], int playerX = 1, int playerY = 1) {
		if (maxX < 0 || maxY < 0) {
			throw "The coordinate cannot be negative";
		}
		this->maxX = maxX;
		this->maxY = maxY;
		size = maxX * maxY;
		tiles = new Tile * [size];

		for (int i = 0; i < size; i++) {
			bool Collision = false;
			for (int j = 0; j < sizeof(collisionable); j++) {
				if (map[i] == collisionable[j]) {
					Collision = true;
					break;
				}
			}
			tiles[i] = new Tile(map[i],Collision);
		}

		GeneratePlayer(playerX, playerY);
	}

	~Map() { // Деструктор для освобождения памяти
		for (int i = 0; i < size; i++) {
			delete tiles[i];
		}
		delete[] tiles;
	}

	string GetMapAsString() {
		string strMap = "";
		for (int i = 0; i < size; i++) {
			strMap = strMap + tiles[i]->GetSymbol();
			if ((i + 1) % maxX == 0) {
				strMap = strMap + '\n';
			}
		}
		return strMap;
	}

	Tile GetTile(int x, int y) {
		return *tiles[maxX * y + x];
	}

	void GeneratePlayer(int x, int y) {
		if (tiles[maxX * y + x]->GetHasCollision()) {
			throw "Player was spawned into tile withon collition";
		}
		else if (x >= maxX || y >= maxY || x < 0 || y < 0) {
			throw "Out of bounce";
		}
		else if (player != nullptr) {
			throw "Player have been spawned yet";
		}

		player = new Player(x,y);
		tiles[maxX * y + x] = player;
	}

	/*
	* Сомнительный метод, нарушающий инкапсуляцию, так как должен быть в классе Player
	* 
	*  P.S. Инкапсуляция != сокрытие
	*/

	void MovePlayer(int right, int down) {
		if (player == nullptr) {
			throw "Player doesn't exist";
		}

		if (tiles[maxX*(player->GetPosition().y + down) + (player->GetPosition().x + right)]->GetHasCollision()) {
			return;
		}
		Player::Position oldPos = player->GetPosition();

		delete tiles[maxX * (oldPos.y + down) + (oldPos.x + right)];
		player = new Player(oldPos.x + right, oldPos.y + down);
		tiles[maxX * (oldPos.y + down) + (oldPos.x + right)] = player;
		
		delete tiles[maxX * oldPos.y + oldPos.x];
		tiles[maxX * oldPos.y + oldPos.x] = new Tile(' ');
	}
};

int main() {
	char* mapArray = new char[60] {
		'#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
		'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#',
		'#', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', '#',
		'#', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', '#',
		'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#',
		'#', '#', '#', '#', '#', '#', '#', '#', '#', '#'
	};

	char coll[] {'#'};

	Map* map = new Map(10, 6,coll,mapArray);
	bool tutorial = false;
	char vector;

	while (true) {
		system("cls");
		cout << map->GetMapAsString() << endl;
		if (!tutorial) {
			cout << "u - up\nd - down\nl - left\nr - right\n";
			tutorial = true;
		}
		cout << ":";
		cin >> vector;
		switch (vector)
		{
			case 'u': {
				map->MovePlayer(0,-1);
				break;
			}
			case 'd': {
				map->MovePlayer(0, 1);
				break;
			}
			case 'l': {
				map->MovePlayer(-1, 0);
				break;
			}
			case 'r': {
				map->MovePlayer(1, 0);
				break;
			}
			default:
				continue;
		}
	}
	return 0;
}