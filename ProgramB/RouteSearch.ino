#include <Wire.h>
#include <ZumoShieldN.h>

// --- グローバル変数 ---
char route[256];
// 外部で定義される入力コマンド配列
extern char destinationCommands[];

const uint8_t NODES = 16;
const uint8_t DIRS = 4;
const uint16_t INF = 999; // 無限大の代わりに大きな値を使用

// ノードIDから座標(x, y)を計算
void getNodePos(uint8_t id, uint8_t *x, uint8_t *y)
{
  *x = id % 4;
  *y = id / 4;
}

// ノードIDから特定の隣接ノードIDを計算
uint8_t getAdjNode(uint8_t id, uint8_t index, uint8_t *adj_count)
{
  uint8_t x, y;
  getNodePos(id, &x, &y);
  int8_t DX[] = {0, 1, 0, -1}; // 北・東・南・西の移動ベクトル
  int8_t DY[] = {1, 0, -1, 0};

  uint8_t current_index = 0;
  uint8_t count = 0;
  // 隣接方向をすべてチェック
  for (uint8_t i = 0; i < DIRS; ++i)
  {
    uint8_t nx = x + DX[i];
    uint8_t ny = y + DY[i];
    if (nx < 4 && ny < 4)
    { // グリッド範囲内かチェック
      if (current_index == index)
      {
        // 先に全体の隣接数を計算しておく
        for (uint8_t j = 0; j < DIRS; ++j)
        {
          if ((x + DX[j] < 4) && (y + DY[j] < 4))
            (*adj_count)++;
        }
        return ny * 4 + nx;
      }
      current_index++;
    }
  }
  return 255; // エラー
}

// --- 文字をノードIDに変換 ---
uint8_t node_id(char c)
{
  if (c >= 'a' && c <= 'z')
  {
    c = c - 'a' + 'A'; // 小文字を大文字に変換
  }

  if (c >= '0' && c <= '9')
  {
    return c - '0';
  }

  return 10 + (c - 'A');
}

// --- 経路探索とコマンド生成 ---

// 2ノード間の移動コマンドと新しい方角を計算
uint8_t direction_to_commands(uint8_t from_dir, uint8_t from, uint8_t to, uint8_t *new_dir, char *cmds)
{
  uint8_t fx, fy, tx, ty;
  getNodePos(from, &fx, &fy);
  getNodePos(to, &tx, &ty);
  int8_t dx = tx - fx;
  int8_t dy = ty - fy;

  // 移動方向を判定し new_dir に設定
  if (dx == 0 && dy == 1)
    *new_dir = 0;
  else if (dx == 1 && dy == 0)
    *new_dir = 1;
  else if (dx == 0 && dy == -1)
    *new_dir = 2;
  else if (dx == -1 && dy == 0)
    *new_dir = 3;
  else
    return 0;

  uint8_t len = 0;
  // 回転方向の差分を求める
  uint8_t diff = (*new_dir - from_dir + 4) % 4;
  if (diff == 1)
    cmds[len++] = 'r';
  else if (diff == 3)
    cmds[len++] = 'l';
  else if (diff == 2)
    cmds[len++] = 'u';
  cmds[len++] = 'f';
  return len;
}

// ダイクストラ法
uint8_t dijkstra(uint8_t start, uint8_t goal, uint8_t init_dir, uint8_t path[][2], const uint8_t *cmd_cost_map)
{
  // --- 内部の配列を最適化 ---
  uint16_t dist[NODES][DIRS];
  // 前のノード(4bit)と向き(2bit)を1バイトにパックして格納
  uint8_t prev[NODES][DIRS];
  // 訪問済みフラグをビットフィールドで管理 (16x4のbool配列 -> 16バイトのuint8_t配列)
  uint8_t visited[NODES] = {0};

  // 初期化
  for (uint8_t i = 0; i < NODES; ++i)
  {
    for (uint8_t j = 0; j < DIRS; ++j)
    {
      dist[i][j] = INF;
      prev[i][j] = 255; // 未訪問マーク
    }
  }

  dist[start][init_dir] = 0;

  while (true)
  {
    // 未訪問の中で最小コストのノードを探す
    uint16_t min_cost = INF;
    uint8_t u = 255, dir_u = 255;
    for (uint8_t i = 0; i < NODES; ++i)
    {
      for (uint8_t d = 0; d < DIRS; ++d)
      {
        if (!(visited[i] & (1 << d)) && dist[i][d] < min_cost)
        {
          min_cost = dist[i][d];
          u = i;
          dir_u = d;
        }
      }
    }
    if (u == 255 || u == goal)
      break;
    visited[u] |= (1 << dir_u);

    // 隣接ノードを探索
    uint8_t adj_count = 0;
    for (uint8_t i = 0;; ++i)
    {
      uint8_t v = getAdjNode(u, i, &adj_count);
      if (i >= adj_count)
        break;

      uint8_t new_dir;
      char cmds[4];
      uint8_t cmd_len = direction_to_commands(dir_u, u, v, &new_dir, cmds);
      uint16_t cost = 0;

      // コマンドに応じたコストを加算
      for (uint8_t j = 0; j < cmd_len; ++j)
      {
        if (cmds[j] == 'f')
          cost += cmd_cost_map[0];
        else if (cmds[j] == 'r')
          cost += cmd_cost_map[1];
        else if (cmds[j] == 'l')
          cost += cmd_cost_map[2];
        else if (cmds[j] == 'u')
          cost += cmd_cost_map[3];
      }
      // 最短距離を更新
      if (dist[v][new_dir] > dist[u][dir_u] + cost)
      {
        dist[v][new_dir] = dist[u][dir_u] + cost;
        prev[v][new_dir] = (u << 2) | dir_u;
      }
    }
  }

  // 最もコストの小さいゴール状態を選択
  uint8_t best_dir = 0;
  uint16_t best_cost = INF;
  for (uint8_t d = 0; d < DIRS; ++d)
  {
    if (dist[goal][d] < best_cost)
    {
      best_cost = dist[goal][d];
      best_dir = d;
    }
  }

  // 経路を復元
  uint8_t len = 0;
  uint8_t v = goal, d = best_dir;
  while (v != 255 && d != 255)
  {
    path[len][0] = v;
    path[len][1] = d;
    len++;
    uint8_t packed_prev = prev[v][d];
    if (packed_prev == 255)
      break;
    v = packed_prev >> 2;
    d = packed_prev & 0x03;
  }

  // 経路を逆順にする
  for (uint8_t i = 0; i < len / 2; ++i)
  {
    uint8_t t0 = path[i][0], t1 = path[i][1];
    path[i][0] = path[len - 1 - i][0];
    path[i][1] = path[len - 1 - i][1];
    path[len - 1 - i][0] = t0;
    path[len - 1 - i][1] = t1;
  }
  return len;
}

void doRouteSearch()
{
  // 0:f, 1:r, 2:l, 3:u
  uint8_t cmd_cost_map[4] = {1, 1, 1, 2};

  uint8_t input_len = 0;
  uint8_t nodes[16];
  // 入力コマンドをノードIDに変換
  while (destinationCommands[input_len] != '\0' && input_len < 15)
  {
    nodes[input_len] = node_id(destinationCommands[input_len]);
    ++input_len;
  }
  nodes[input_len] = '\0';

  int route_len = 0;
  uint8_t curr_id = 0;
  uint8_t curr_dir = 0;
  route[route_len++] = 'f';

  // 各目的地に順に移動
  for (uint8_t i = 0; i < input_len; ++i)
  {
    uint8_t target = nodes[i];
    if (target == curr_id)
      continue; // 既に同じノードならスキップ
    uint8_t path[16][2];
    uint8_t path_len = dijkstra(curr_id, target, curr_dir, path, cmd_cost_map);

    // 経路に沿ってコマンドを追加
    for (uint8_t j = 1; j < path_len; ++j)
    {
      uint8_t from = path[j - 1][0], dir = path[j - 1][1];
      uint8_t to = path[j][0], new_dir;
      char cmds[4];
      uint8_t cmd_len = direction_to_commands(dir, from, to, &new_dir, cmds);
      for (uint8_t k = 0; k < cmd_len; ++k)
      {
        if (route_len < 255)
          route[route_len++] = cmds[k];
      }
      curr_dir = new_dir;
    }
    curr_id = target;
  }
  route[route_len++] = '.';

  // 結果を出力
  route[route_len] = '\0';
  Serial.println("Route commands:");
  Serial.println(route);

  // 総コストを計算
  int total_cost = 0;
  for (int i = 0; i < route_len; ++i)
  {
    if (route[i] == 'f')
      total_cost += cmd_cost_map[0];
    else if (route[i] == 'r')
      total_cost += cmd_cost_map[1];
    else if (route[i] == 'l')
      total_cost += cmd_cost_map[2];
    else if (route[i] == 'u')
      total_cost += cmd_cost_map[3];
  }
  Serial.println("Total cost:");
  Serial.println(total_cost);
}