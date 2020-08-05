#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <set>
#include <algorithm>
#include <random>


using namespace std;

int const TILE_SIZE = 8;
int const N = 99;
int const M = 99;

struct cell{
    int i;
    int j;
    bool wall;
    int num;


    bool operator==(cell const &c) const{
        return i == c.i && j == c.j;
    }

    bool operator!=(cell const &c) const{
        return !(*this == c);
    }

    bool operator<(cell const &c) const{
        return i <= c.i;
    }
};


class grid{
    int n;
    int m;
    vector<vector<cell>> g;
public:
    cell s;
    cell f;
    grid(int n_, int m_) : n(n_), m(m_){
        g = vector<vector<cell>>(n, vector<cell> (m));
        int c = 0;
        s = cell{0, 0, false};
        f = cell{n - 1, m - 1, false};
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++)
                g[i][j] = cell{i, j, true, c++};
    }
    //~grid() = default;

    void set(int i, int j, bool w){
        g[i][j] = cell{i, j, w, g[i][j].num};
    }

    int size_n() const{
        return n;
    }
    int size_m() const{
        return m;
    }
    cell at(int i, int j){
        return g[i][j];
    }
};

int h(cell const &a, cell const &b){
    return abs(a.i - b.i) + abs(a.j - b.j);
}
int const DSU_N = 100000;
vector<int> p_(DSU_N);

int p(int u){
    return (p_[u] == u ? u : p_[u] = p(p_[u]));
}

void unite(int a, int b){
    a = p(a);
    b = p(b);
    if (rand() & 1) p_[a] = b;
    else p_[b] = a;
}

grid gen_labirynth(){
    for (int i = 0; i < DSU_N; i++) p_[i] = i;
    auto g = grid(N, M);
    vector<int> di = {0, 2};
    vector<int> dj = {2, 0};
    vector<pair<int, pair<cell, cell>>> edges;
    for (int i = 0; i < N; i += 2){
        for (int j = 0; j < M; j += 2){
            for (int c = 0; c < 2; c++){
                int ni = i + di[c];
                int nj = j + dj[c];
                if (ni < 0 || ni >= N || nj < 0 || nj >= M) continue;
                int rnd = rand();
                edges.push_back({rnd, {g.at(i, j), g.at(ni, nj)}});
                edges.push_back({rnd, {g.at(ni, nj), g.at(i, j)}});
            }
        }
    }
    sort(edges.begin(), edges.end(), [](pair<int, pair<cell, cell>> a, pair<int, pair<cell, cell>> b){return a.first < b.first;});
    for (auto c : edges){
        cell c1 = c.second.first;
        cell c2 = c.second.second;
        if (p(c1.num) == p(c2.num)){
            continue;
        }
        else{
            unite(c1.num, c2.num);
            g.set(c1.i, c1.j, false);
            g.set(c2.i, c2.j, false);
            g.set((c1.i + c2.i) / 2, (c1.j + c2.j) / 2, false);
        }
    }
    return g;
}


vector<cell> a_star_search(grid &g){
    cell s = g.s;
    cell f = g.f;
    int n = g.size_n();
    int m = g.size_m();
    int INF = 2140000000;
    vector<vector<cell>> prev_cell(n, vector<cell> (m));
    vector<cell> p;
    vector<vector<int>> d(n, vector<int> (m, INF));
    set<pair<int, cell>> q;
    q.insert({h(s, f), s});
    vector<int> di = {0, 0, 1, -1};
    vector<int> dj = {1, -1, 0, 0};
    d[s.i][s.j] = 0;
    while (!q.empty() && d[f.i][f.j] == INF){
        cell current_cell = q.begin()->second;
        q.erase(q.begin());
        for (int i = 0; i < di.size(); i++){
            int new_i = current_cell.i + di[i];
            int new_j = current_cell.j + dj[i];
            if (new_i < 0 || new_i >= n || new_j < 0 || new_j >= m ||
                g.at(new_i, new_j).wall || d[new_i][new_j] != INF)
                continue;
            else{
                cell next_cell = g.at(new_i, new_j);
                d[new_i][new_j] = d[current_cell.i][current_cell.j] + 1;
                prev_cell[new_i][new_j] = current_cell;
                q.insert({d[new_i][new_j] + h(next_cell, f), next_cell});
            }
        }
    }
    p.push_back(f);
    for (auto pi = prev_cell[f.i][f.j]; pi != s; pi = prev_cell[pi.i][pi.j]) p.push_back(pi);
    p.push_back(s);
    reverse(p.begin(), p.end());
    return p;
}


bool init(){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}


void draw_grid(grid &g, SDL_Surface *sur){
    vector<cell> path = a_star_search(g);
    for (int i = 0; i < N; i++){
        for (int j = 0; j < M; j++){
            SDL_Rect * rct = new SDL_Rect{j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            if (g.at(i, j).wall){
                SDL_FillRect(sur, rct, SDL_MapRGB(sur->format, 0, 0, 0));
            }
            else{
                SDL_FillRect(sur, rct, SDL_MapRGB(sur->format, 255, 255, 255));
            }
            delete rct;
        }
    }
    for (auto c : path){
        int i = c.i;
        int j = c.j;
        SDL_Rect * rct = new SDL_Rect{j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        SDL_FillRect(sur, rct, SDL_MapRGB(sur->format, rand() % 255, rand() % 255, rand() % 255));
        delete rct;
    }
    SDL_Rect * rct_s = new SDL_Rect{g.s.j * TILE_SIZE, g.s.i * TILE_SIZE, TILE_SIZE, TILE_SIZE};
    SDL_FillRect(sur, rct_s, SDL_MapRGB(sur->format, 255, 0, 0));
    SDL_Rect * rct_f = new SDL_Rect{g.f.j * TILE_SIZE, g.f.i * TILE_SIZE, TILE_SIZE, TILE_SIZE};
    SDL_FillRect(sur, rct_f, SDL_MapRGB(sur->format, 0, 255, 0));
    delete rct_s;
    delete rct_f;
}

int main(int argc, char * argv[])
{
    srand(time(NULL));
    if (!init()) return 1;
    SDL_Window *win = SDL_CreateWindow("A*", 100, 100, M * TILE_SIZE, N * TILE_SIZE, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Surface  *sur = SDL_CreateRGBSurface(0, M * TILE_SIZE, N * TILE_SIZE, 32, 0, 0, 0, 0);
    SDL_Event e;
    grid g = gen_labirynth();
    bool quit = false;
    while (!quit)
    {
        // Обработка событий
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            if (e.type == SDL_KEYDOWN)
            {
                g = gen_labirynth();
            }
            if (e.type == SDL_MOUSEBUTTONDOWN){
                auto bt = e.button;
                if (bt.button == SDL_BUTTON_LEFT){
                    int ni = bt.y / TILE_SIZE;
                    int nj = bt.x / TILE_SIZE;
                    if (!g.at(ni, nj).wall && g.at(ni, nj) != g.f)
                        g.s= cell{ni, nj, false, g.at(ni, nj).num};
                }
                else if (e.button.button == SDL_BUTTON_RIGHT){
                    int ni = bt.y / TILE_SIZE;
                    int nj = bt.x / TILE_SIZE;
                    if (!g.at(ni, nj).wall && g.at(ni, nj) != g.s)
                        g.f= cell{ni, nj, false, g.at(ni, nj).num};
                }
            }
        }
        draw_grid(g, sur);
        SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, sur);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);
        SDL_DestroyTexture(tex);
        SDL_Delay(17);
    }
    return 0;
}