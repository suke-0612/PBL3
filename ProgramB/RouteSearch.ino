#include <Wire.h>
#include <ZumoShieldN.h>

const int NODES = 16;
const int DIRS = 4;
const int INF = 999;
extern char destinationCommands[];

int DX[4] = {0, 1, 0, -1};
int DY[4] = {1, 0, -1, 0};
int CMD_COST[128];
char route[20];
int pos[16][2] = {
  {0,0}, {1,0}, {2,0}, {3,0},
  {0,1}, {1,1}, {2,1}, {3,1},
  {0,2}, {1,2}, {2,2}, {3,2},
  {0,3}, {1,3}, {2,3}, {3,3}
};

int adj[16][4] = {
  {1,4}, {0,2,5}, {1,3,6}, {2,7},
  {0,5,8}, {1,4,6,9}, {2,5,7,10}, {3,6,11},
  {4,9,12}, {5,8,10,13}, {6,9,11,14}, {7,10,15},
  {8,13}, {9,12,14}, {10,13,15}, {11,14}
};

int deg[16] = {
  2, 3, 3, 2,
  3, 4, 4, 3,
  3, 4, 4, 3,
  2, 3, 3, 2
};

int node_id(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  return 10 + (c - 'A');
}

int direction_to_commands(int from_dir, int from, int to, int* new_dir, char* cmds) {
  int fx = pos[from][0], fy = pos[from][1];
  int tx = pos[to][0], ty = pos[to][1];
  int dx = tx - fx, dy = ty - fy;
  if (dx == 0 && dy == 1) *new_dir = 0;
  else if (dx == 1 && dy == 0) *new_dir = 1;
  else if (dx == 0 && dy == -1) *new_dir = 2;
  else if (dx == -1 && dy == 0) *new_dir = 3;
  else return 0;

  int len = 0;
  int diff = (*new_dir - from_dir + 4) % 4;
  if (diff == 1) cmds[len++] = 'r';
  else if (diff == 3) cmds[len++] = 'l';
  else if (diff == 2) cmds[len++] = 'u';
  cmds[len++] = 'f';
  return len;
}

int dijkstra(int start, int goal, int init_dir, int path[][2]) {
  bool visited[16][4] = {};
  int dist[16][4];
  int prev_node[16][4], prev_dir[16][4];

  for (int i = 0; i < 16; ++i)
    for (int j = 0; j < 4; ++j) {
      dist[i][j] = INF;
      prev_node[i][j] = prev_dir[i][j] = -1;
    }

  dist[start][init_dir] = 0;

  while (true) {
    int min_cost = INF, u = -1, dir_u = -1;
    for (int i = 0; i < 16; ++i)
      for (int d = 0; d < 4; ++d)
        if (!visited[i][d] && dist[i][d] < min_cost) {
          min_cost = dist[i][d];
          u = i; dir_u = d;
        }
    if (u == -1) break;
    visited[u][dir_u] = true;
    if (u == goal) break;

    for (int i = 0; i < deg[u]; ++i) {
      int v = adj[u][i];
      int new_dir;
      char cmds[4];
      int cmd_len = direction_to_commands(dir_u, u, v, &new_dir, cmds);
      int cost = 0;
      for (int j = 0; j < cmd_len; ++j) cost += CMD_COST[cmds[j]];
      if (dist[v][new_dir] > dist[u][dir_u] + cost) {
        dist[v][new_dir] = dist[u][dir_u] + cost;
        prev_node[v][new_dir] = u;
        prev_dir[v][new_dir] = dir_u;
      }
    }
  }

  int best_dir = 0, best_cost = INF;
  for (int d = 0; d < 4; ++d) {
    if (dist[goal][d] < best_cost) {
      best_cost = dist[goal][d];
      best_dir = d;
    }
  }

  int len = 0;
  int v = goal, d = best_dir;
  while (v != -1 && d != -1) {
    path[len][0] = v;
    path[len][1] = d;
    int pv = prev_node[v][d], pd = prev_dir[v][d];
    v = pv; d = pd;
    len++;
  }

  for (int i = 0; i < len / 2; ++i) {
    int t0 = path[i][0], t1 = path[i][1];
    path[i][0] = path[len - 1 - i][0];
    path[i][1] = path[len - 1 - i][1];
    path[len - 1 - i][0] = t0;
    path[len - 1 - i][1] = t1;
  }
  return len;
}

void doRouteSearch() {
  CMD_COST['f'] = 1;
  CMD_COST['r'] = 1;
  CMD_COST['l'] = 1;
  CMD_COST['u'] = 2;

  int input_len = 0;
  int nodes[32];
  while (destinationCommands[input_len] != '\0') {
    nodes[input_len] = node_id(destinationCommands[input_len]);
    ++input_len;
  }

  char result[512];
  int result_len = 0;
  int curr_id = 0;
  int curr_dir = 0;
  result[result_len++] = 'f';

  for (int i = 0; i < input_len; ++i) {
    int target = nodes[i];
    if (target == curr_id) continue;
    int path[64][2];
    int path_len = dijkstra(curr_id, target, curr_dir, path);
    for (int j = 1; j < path_len; ++j) {
      int from = path[j - 1][0], dir = path[j - 1][1];
      int to = path[j][0], new_dir;
      char cmds[4];
      int cmd_len = direction_to_commands(dir, from, to, &new_dir, cmds);
      for (int k = 0; k < cmd_len; ++k) result[result_len++] = cmds[k];
      curr_dir = new_dir;
    }
    curr_id = target;
  }

  result[result_len] = '\0';
  Serial.println("Route commands:");
  Serial.println(result);

  int total_cost = 0;
  for (int i = 0; i < result_len; ++i)
    total_cost += CMD_COST[result[i]];
  Serial.println("Total cost:");
  Serial.println(total_cost);
}
 