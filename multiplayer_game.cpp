#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <vector>
#include <unordered_map>
#include <algorithm>
using namespace std;

class SkipList {
private:
    struct Node {
        int value;
        Node** next;
        int level;

        Node(int value, int level) {
            this->value = value;
            this->level = level;
            next = new Node*[level + 1];
            for (int i = 0; i <= level; i++) {
                next[i] = nullptr;
            }
        }

        ~Node() {
            delete[] next;
        }
    };

    Node* head;
    int maxLevel;
    float p;
    int level;
    unordered_map<int, int> players;

    int randomLevel() {
        int lvl = 0;
        while ((float)rand() / RAND_MAX < p && lvl < maxLevel)
            lvl++;
        return lvl;
    }

public:
    SkipList(int maxLevel = 16, float p = .5) {
        this->maxLevel = maxLevel;
        this->p = p;
        level = 0;
        head = new Node(numeric_limits<int>::min(), maxLevel);
    }

    ~SkipList() {
        Node* current = head;
        while (current != nullptr) {
            Node* nextNode = current->next[0];
            delete current;
            current = nextNode;
        }
    }

    void insert(int value) {
        Node* update[maxLevel + 1];
        Node* current = head;

        for (int i = level; i >= 0; i--) {
            while (current->next[i] != nullptr && current->next[i]->value < value) {
                current = current->next[i];
            }
            update[i] = current;
        }

        int newLevel = randomLevel();

        if (newLevel > level) {
            for (int i = level + 1; i <= newLevel; i++) {
                update[i] = head;
            }
            level = newLevel;
        }

        Node* newNode = new Node(value, newLevel);
        for (int i = 0; i <= newLevel; i++) {
            newNode->next[i] = update[i]->next[i];
            update[i]->next[i] = newNode;
        }
    }

    bool remove(int value) {
        Node* update[maxLevel + 1];
        Node* current = head;

        for (int i = level; i >= 0; i--) {
            while (current->next[i] != nullptr && current->next[i]->value < value) {
                current = current->next[i];
            }
            update[i] = current;
        }

        current = current->next[0];

        if (current != nullptr && current->value == value) {
            for (int i = 0; i <= level; i++) {
                if (update[i]->next[i] != current) {
                    break;
                }
                update[i]->next[i] = current->next[i];
            }

            while (level > 0 && head->next[level] == nullptr) {
                level--;
            }

            delete current;
            return true;
        }
        return false;
    }
    bool search(int value) {
        Node* current = head;

        for (int i = level; i >= 0; i--) {
            while (current->next[i] != nullptr && current->next[i]->value < value) {
                current = current->next[i];
            }
        }

        current = current->next[0];
        return (current != nullptr && current->value == value);
    }

    void display() {
        cout << "\nSkip List:" << endl;
        for (int i = level; i >= 0; i--) {
            Node* current = head->next[i];
            cout << "Level " << i << ": ";
            while (current != nullptr) {
                cout << current->value << " ";
                current = current->next[i];
            }
            cout << endl;
        }
    }

    void joinGame(int player) {
        if (players.find(player) != players.end()) {
            cout << "Player " << player << " is already in the game." << endl;
            return;
        }

        int score;
        cout << "Enter score for Player " << player << ": ";
        cin >> score;
        players[player] = score;
        insert(score);
        cout << "Player " << player << " with score " << score << " has joined the game." << endl;
    }

    void leaveGame(int player) {
        if (players.find(player) == players.end()) {
            cout << "Player " << player << " not found." << endl;
            return;
        }

        int score = players[player];
        if (remove(score)) {
            players.erase(player);
            cout << "Player " << player << " with score " << score << " has left the game." << endl;
        }
    }

    void viewScore(int player) {
        if (players.find(player) != players.end()) {
            cout << "Player " << player << " has score: " << players[player] << endl;
        } else {
            cout << "Player " << player << " is not in the game." << endl;
        }
    }

    void getTopPlayers(int N) {
        vector<pair<int, int>> playerList(players.begin(), players.end());
        sort(playerList.begin(), playerList.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
            return a.second > b.second;
        });

        cout << "Top " << N << " Players:\n";
        for (int i = 0; i < N && i < playerList.size(); i++) {
            cout << "Player " << playerList[i].first << " with score: " << playerList[i].second << endl;
        }
    }

    void updateScore(int player, int oldScore, int newScore) {
        if (players.find(player) == players.end() || players[player] != oldScore) {
            cout << "Player " << player << " with score " << oldScore << " not found." << endl;
            return;
        }
        remove(oldScore);
        insert(newScore);
        players[player] = newScore;
        cout << "Player " << player << "'s score updated to " << newScore << "." << endl;
    }
};

int main() {
    srand(time(0));
    SkipList scores(5, .5);

    int choice;
    do {
        cout << "\n1. Join Game\n2. Leave Game\n3. View Player's Score\n4. Get Top Players\n5. Update Player's Score\n0. Exit\nEnter your choice: ";
        cin >> choice;

        int player, oldScore, newScore, N;
        switch (choice) {
        case 1:
            cout << "Enter Player Number: ";
            cin >> player;
            scores.joinGame(player);
            break;
        case 2:
            cout << "Enter Player Number: ";
            cin >> player;
            scores.leaveGame(player);
            break;
        case 3:
            cout << "Enter Player Number: ";
            cin >> player;
            scores.viewScore(player);
            break;
        case 4:
            cout << "Enter number of top players to retrieve: ";
            cin >> N;
            scores.getTopPlayers(N);
            break;
        case 5:
            cout << "Enter Player Number: ";
            cin >> player;
            cout << "Enter Old Score: ";
            cin >> oldScore;
            cout << "Enter New Score: ";
            cin >> newScore;
            scores.updateScore(player, oldScore, newScore);
            break;
        }
    } while (choice != 0);

    return 0;
}
