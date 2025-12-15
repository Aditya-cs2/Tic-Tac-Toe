#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <limits>
using namespace std;


mt19937 rng((unsigned)time(nullptr));
int rint(int lo, int hi) { uniform_int_distribution<int> d(lo, hi); return d(rng); }
bool chance(int pct) { return rint(1, 100) <= pct; }


struct Character {
    string name;
    string cls; 
    int hp, atk, def;
};

enum Ability {
    NONE,
    SPIRIT_VISION,     
    PAPER_FIGURINE,    
    PSYCHOLOGICAL_CUE, 
    FIREBALL,          
    FOG_OF_MYSTERY     
};

struct Enemy {
    string name;
    int sequence;      
    int hp, atk, def;
    Ability a1, a2;    
    bool boss;
};


string abilityName(Ability a) {
    switch (a) {
        case SPIRIT_VISION: return "Spirit Vision";
        case PAPER_FIGURINE: return "Paper Figurine Substitute";
        case PSYCHOLOGICAL_CUE: return "Psychological Cue";
        case FIREBALL: return "Mystic Fireball";
        case FOG_OF_MYSTERY: return "Fog of Mystery";
        default: return "None";
    }
}

string abilityHelp(Ability a) {
    switch (a) {
        case SPIRIT_VISION:
            return "Spirit Vision: The enemy senses danger better. (Enemy gets +2 DEF for the next tic-tac-toe round.)";
        case PAPER_FIGURINE:
            return "Paper Figurine Substitute: A decoy takes part of the hit. (If you damage them this round, damage is reduced by 2, min 0.)";
        case PSYCHOLOGICAL_CUE:
            return "Psychological Cue: Mental pressure makes your mistakes worse. (If enemy wins the round, you take +2 extra damage.)";
        case FIREBALL:
            return "Mystic Fireball: Direct supernatural attack. (You instantly take 1-4 damage before the round.)";
        case FOG_OF_MYSTERY:
            return "Fog of Mystery: The boss becomes harder to harm. (Boss gets +4 DEF for the next round.)";
        default:
            return "No special ability.";
    }
}


void printBoard(const vector<char>& b) {
    cout << "\n " << b[0] << " | " << b[1] << " | " << b[2] << "\n";
    cout << "---+---+---\n";
    cout << " " << b[3] << " | " << b[4] << " | " << b[5] << "\n";
    cout << "---+---+---\n";
    cout << " " << b[6] << " | " << b[7] << " | " << b[8] << "\n\n";
}

int winner(const vector<char>& b) {
    int lines[8][3] = {
        {0,1,2},{3,4,5},{6,7,8},
        {0,3,6},{1,4,7},{2,5,8},
        {0,4,8},{2,4,6}
    };
    for (auto &ln : lines) {
        char a=b[ln[0]], c=b[ln[1]], d=b[ln[2]];
        if (a!=' ' && a==c && c==d) return (a=='X') ? 1 : 2; 
    }
    bool full=true;
    for (char x: b) if (x==' ') full=false;
    if (full) return 3; 
    return 0;
}

int playerMove(const vector<char>& b) {
    while (true) {
        cout << "Pick a spot (1-9): ";
        int p; cin >> p;
        if (!cin) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        if (p < 1 || p > 9) continue;
        int i = p - 1;
        if (b[i] != ' ') continue;
        return i;
    }
}

int enemyMove(const vector<char>& b) {
    vector<int> free;
    for (int i=0;i<9;i++) if (b[i]==' ') free.push_back(i);
    return free[rint(0, (int)free.size()-1)];
}


int dmg(int atk, int def) {
    int d = atk - def;
    return (d < 0) ? 0 : d;
}

struct RoundMod {
    int enemyDefBonus = 0; 
    bool paperOn = false;  
    bool psychOn = false;  
};

void enemyAbility(Character& player, const Enemy& e, RoundMod& mod) {
    mod.enemyDefBonus = 0;
    mod.paperOn = false;
    mod.psychOn = false;

    
    if (!chance(45)) return;

    Ability chosen = e.a1;
    if (e.boss) chosen = chance(50) ? e.a1 : e.a2;

    cout << ">> " << e.name << " activates: " << abilityName(chosen) << "\n";
    cout << "   " << abilityHelp(chosen) << "\n";

    if (chosen == SPIRIT_VISION) mod.enemyDefBonus = 2;
    else if (chosen == PAPER_FIGURINE) mod.paperOn = true;
    else if (chosen == PSYCHOLOGICAL_CUE) mod.psychOn = true;
    else if (chosen == FIREBALL) {
        int hit = rint(1,4);
        cout << ">> Fireball hits you for " << hit << "!\n";
        player.hp -= hit;
        if (player.hp < 0) player.hp = 0;
    }
    else if (chosen == FOG_OF_MYSTERY) mod.enemyDefBonus = 4;
}

bool battle(Character& player, const Enemy& templ) {
    Character enemy{templ.name, "Enemy", templ.hp, templ.atk, templ.def};

    cout << "\n==============================\n";
    cout << "BATTLE: " << player.name << " vs " << enemy.name << "\n";
    cout << "Enemy Sequence: " << templ.sequence << "\n";
    if (templ.boss) cout << "WARNING: This is a low-Sequence entity.\n";
    cout << "Enemy ability: " << abilityName(templ.a1) << "\n";
    if (templ.boss) cout << "Boss extra: " << abilityName(templ.a2) << "\n";
    cout << "Tip: Abilities are explained whenever they trigger.\n";
    cout << "==============================\n";

    while (player.hp > 0 && enemy.hp > 0) {
        cout << "\nYour HP:" << player.hp << " ATK:" << player.atk << " DEF:" << player.def << "\n";
        cout << "Enemy HP:" << enemy.hp << " ATK:" << enemy.atk << " DEF:" << enemy.def << "\n";

        RoundMod mod;
        enemyAbility(player, templ, mod);
        if (player.hp <= 0) break;

        vector<char> b(9, ' ');
        cout << "\nYou are X. Enemy is O.\n";
        cout << "Positions: 1 2 3 / 4 5 6 / 7 8 9\n";

        bool turnPlayer = true;
        while (winner(b) == 0) {
            printBoard(b);
            if (turnPlayer) {
                b[playerMove(b)] = 'X';
            } else {
                int mv = enemyMove(b);
                b[mv] = 'O';
                cout << enemy.name << " plays " << (mv+1) << "\n";
            }
            turnPlayer = !turnPlayer;
        }

        printBoard(b);
        int w = winner(b);

        int enemyDefNow = enemy.def + mod.enemyDefBonus;

        if (w == 1) {
            int dealt = dmg(player.atk, enemyDefNow);
            if (mod.paperOn) {
                dealt = max(0, dealt - 2);
                cout << ">> Paper Figurine reduces your damage by 2.\n";
            }
            cout << ">> You won the round! You deal " << dealt << "\n";
            enemy.hp -= dealt;
        } else if (w == 2) {
            int taken = dmg(enemy.atk, player.def);
            if (mod.psychOn) {
                taken += 2;
                cout << ">> Psychological Cue adds +2 damage.\n";
            }
            cout << ">> Enemy won the round! You take " << taken << "\n";
            player.hp -= taken;
        } else {
            cout << ">> Draw. No damage.\n";
        }

        if (player.hp < 0) player.hp = 0;
        if (enemy.hp < 0) enemy.hp = 0;
    }

    if (player.hp <= 0) {
        cout << "\n*** You lost to " << templ.name << " ***\n";
        return false;
    }
    cout << "\n*** You defeated " << templ.name << "! ***\n";
    return true;
}


void eventChoice(Character& p) {
    cout << "\n--- EVENT: Safe House (Nighthawks) ---\n";
    cout << "No-Reader Note: Nighthawks are investigators of supernatural incidents.\n";
    cout << "1) Receive treatment (+6 HP)\n";
    cout << "2) Train rituals (+1 ATK, -2 HP)\n";
    int c;
    while (true) {
        cout << "Choose 1 or 2: ";
        cin >> c;
        if (cin && (c==1 || c==2)) break;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    if (c==1) p.hp += 6;
    else { p.atk += 1; p.hp -= 2; }
    if (p.hp < 0) p.hp = 0;
}

void eventPotion(Character& p) {
    cout << "\n--- EVENT: Low-Sequence Potion ---\n";
    cout << "No-Reader Note: Potions give powers but are risky. Here it's safe and simple.\n";
    int roll = rint(1,3);
    if (roll==1) { p.hp += 4; cout << "+4 HP\n"; }
    else if (roll==2) { p.def += 1; cout << "+1 DEF\n"; }
    else { p.atk += 1; cout << "+1 ATK\n"; }
}

void eventCorruption(Character& p) {
    cout << "\n--- EVENT: Whispering Corruption ---\n";
    cout << "No-Reader Note: Strange whispers can weaken your mind/body.\n";
    p.hp -= rint(1,4);
    p.def -= 1;
    if (p.hp < 0) p.hp = 0;
    if (p.def < 0) p.def = 0;
    cout << "You lose some HP and -1 DEF (not below 0).\n";
}


int main() {
    cout << "=== Lord of Mysteries: Tic-Tac-Toe Campaign (Simple) ===\n\n";

    
    cout << "HOW THIS GAME WORKS (No spoilers):\n";
    cout << "- You fight 5 battles.\n";
    cout << "- Each battle is many tic-tac-toe rounds until someone hits 0 HP.\n";
    cout << "- If you win a round, enemy takes damage = your ATK - enemy DEF (min 0).\n";
    cout << "- If enemy wins a round, you take damage = enemy ATK - your DEF (min 0).\n";
    cout << "- Enemies move randomly.\n";
    cout << "- Enemy abilities are explained when used.\n\n";

    
    cout << "SEQUENCE LEVELS (No-spoiler explanation):\n";
    cout << "- Power is ranked by Sequence numbers.\n";
    cout << "- Higher numbers = weaker (Sequence 9 is low level).\n";
    cout << "- Lower numbers = stronger (Sequence 1 is very strong).\n";
    cout << "- Bosses are usually very low Sequence.\n\n";

    Character player;
    cout << "Enter your name: ";
    getline(cin, player.name);
    if (player.name.empty()) player.name = "Klein-ish Rookie";

    cout << "\nChoose your role (simple re-skin for the assignment classes):\n";
    cout << "1) Nighthawk (like 'Paladin' - tougher defense)\n";
    cout << "2) Potion Master (like 'Alchemist' - higher attack)\n";
    int cls=0;
    while (true) {
        cout << "Enter 1 or 2: ";
        cin >> cls;
        if (cin && (cls==1 || cls==2)) break;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    int baseHP, baseATK, baseDEF;
    if (cls==1) {
        player.cls = "Nighthawk";
        baseHP=26; baseATK=6; baseDEF=4;
    } else {
        player.cls = "Potion Master";
        baseHP=22; baseATK=8; baseDEF=2;
    }

    
    Enemy e1{"Rampaging Marionette", 8, 10, 5, 1, PAPER_FIGURINE, NONE, false};
    Enemy e2{"Secret Order Agent",   7, 12, 6, 2, PSYCHOLOGICAL_CUE, NONE, false};

    
    Enemy forest3{"Mutated Hound",   6, 13, 7, 2, SPIRIT_VISION, NONE, false};
    Enemy forest4{"Creeping Vine",   6, 14, 7, 3, PAPER_FIGURINE, NONE, false};

    Enemy ruins3{"Haunted Armor",    6, 13, 6, 3, SPIRIT_VISION, NONE, false};
    Enemy ruins4{"Abyss Rat Swarm",  6, 14, 8, 1, PSYCHOLOGICAL_CUE, NONE, false};

    
    Enemy boss{"Mysterious Demigod", 3, 18, 9, 3, FIREBALL, FOG_OF_MYSTERY, true};

    cout << "\nSTORY BEGIN:\n";
    cout << "You are a " << player.cls << " investigating strange incidents in a foggy industrial city.\n";
    cout << "Survive the campaign and defeat the final threat.\n";
    cout << "If you die, the campaign restarts.\n";

    while (true) {
        
        player.hp = baseHP; player.atk = baseATK; player.def = baseDEF;

        cout << "\n--- Chapter 1: First Case ---\n";
        if (!battle(player, e1)) { cout << "Restarting campaign...\n"; continue; }

        eventChoice(player); 

        cout << "\n--- Chapter 2: Hidden Society ---\n";
        if (!battle(player, e2)) { cout << "Restarting campaign...\n"; continue; }

        eventPotion(player); 

        
        cout << "\n--- BRANCHING PATH ---\n";
        cout << "Pick your investigation route:\n";
        cout << "1) Foggy Docks (more creatures)\n";
        cout << "2) Ancient Ruins (more hauntings)\n";
        int path=0;
        while (true) {
            cout << "Choose 1 or 2: ";
            cin >> path;
            if (cin && (path==1 || path==2)) break;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        if (path==1) {
            cout << "\n--- Chapter 3: Foggy Docks ---\n";
            if (!battle(player, forest3)) { cout << "Restarting campaign...\n"; continue; }

            eventCorruption(player); 

            cout << "\n--- Chapter 4: Things in the Mist ---\n";
            if (!battle(player, forest4)) { cout << "Restarting campaign...\n"; continue; }
        } else {
            cout << "\n--- Chapter 3: Ancient Ruins ---\n";
            if (!battle(player, ruins3)) { cout << "Restarting campaign...\n"; continue; }

            eventCorruption(player); 

            cout << "\n--- Chapter 4: Echoes Below ---\n";
            if (!battle(player, ruins4)) { cout << "Restarting campaign...\n"; continue; }
        }

        cout << "\n--- Final Chapter: The Ritual Site ---\n";
        if (!battle(player, boss)) { cout << "Restarting campaign...\n"; continue; }

        cout << "\n=== YOU WIN! ===\n";
        if (path==1) cout << "The fog thins over the docks. The city breathes again.\n";
        else cout << "The ruins fall silent. The curse recedes into history.\n";
        cout << "A new page turns in your investigation.\n";
        break;
    }

    cout << "\nThanks for playing!\n";
    return 0;
}
