#include<bits/stdc++.h>
using namespace std;
using ll = long long;

const int N = 105;

const long long inf = 1LL << 61;
struct Dinic {
  struct edge {
    int to, rev;
    long long flow, w;
    int id;
  };
  int n, s, t, mxid;
  vector<int> d, flow_through;
  vector<int> done;
  vector<vector<edge>> g;
  Dinic() {}
  Dinic(int _n) {
    n = _n + 10;
    mxid = 0;
    g.resize(n);
  }
  void add_edge(int u, int v, long long w, int id = -1) {
    edge a = {v, (int)g[v].size(), 0, w, id};
    edge b = {u, (int)g[u].size(), 0, 0, -1};//for bidirectional edges cap(b) = w
    g[u].emplace_back(a);
    g[v].emplace_back(b);
    mxid = max(mxid, id);
  }
  bool bfs() {
    d.assign(n, -1);
    d[s] = 0;
    queue<int> q;
    q.push(s);
    while (!q.empty()) {
      int u = q.front();
      q.pop();
      for (auto &e : g[u]) {
        int v = e.to;
        if (d[v] == -1 && e.flow < e.w) d[v] = d[u] + 1, q.push(v);
      }
    }
    return d[t] != -1;
  }
  long long dfs(int u, long long flow) {
    if (u == t) return flow;
    for (int &i = done[u]; i < (int)g[u].size(); i++) {
      edge &e = g[u][i];
      if (e.w <= e.flow) continue;
      int v = e.to;
      if (d[v] == d[u] + 1) {
        long long nw = dfs(v, min(flow, e.w - e.flow));
        if (nw > 0) {
          e.flow += nw;
          g[v][e.rev].flow -= nw;
          return nw;
        }
      }
    }
    return 0;
  }
  long long max_flow(int _s, int _t) {
    s = _s;
    t = _t;
    long long flow = 0;
    while (bfs()) {
      done.assign(n, 0);
      while (long long nw = dfs(s, inf)) flow += nw;
    }
    flow_through.assign(mxid + 10, 0);
    for(int i = 0; i < n; i++) for(auto e : g[i]) if(e.id >= 0) flow_through[e.id] = e.flow;
    return flow;
  }
};
//flow_through[i] = extra flow beyond 'low' sent through edge i
struct LR_Flow {
  Dinic F;
  int n, s, t;
  struct edge {
    int u, v, l, r, id;
  };
  vector<edge> edges;
  LR_Flow() {}
  LR_Flow(int _n) {
    n = _n + 10;
    s = n - 2, t = n - 1;
    edges.clear();
  }
  void add_edge(int u, int v, int l, int r, int id = -1) {
    assert(0 <= l && l <= r);
    edges.push_back({u, v, l, r, id});
    //cerr << u << ' ' << v << ' ' << l << '-' << r << '\n';
  }
  bool feasible(int _s = -1, int _t = -1, int L = -1, int R = -1) {
    if (L != -1) edges.push_back({_t, _s, L, R, -1});
    F = Dinic(n);
    long long target = 0;
    for (auto e : edges) {
      int u = e.u, v = e.v, l = e.l, r = e.r, id = e.id;
      if (l != 0) {
        F.add_edge(s, v, l);
        F.add_edge(u, t, l);
        target += l;
      }
      F.add_edge(u, v, r - l, id);
    }
    auto ans = F.max_flow(s, t);
    if (L != -1) edges.pop_back();
    if (ans < target) return 0; //not feasible
    return 1;
  }
  int max_flow(int _s, int _t) { //-1 means flow is not feasible
    int mx = 1e5 + 9;
    if (!feasible(_s, _t, 0, mx)) return -1;
    return F.max_flow(_s, _t);
  }
  int min_flow(int _s, int _t) { //-1 means flow is not feasible
    int mx = 1e9;
    int ans = -1, l = 0, r = mx;
    while (l <= r) {
      int mid = l + r >> 1;
      if (feasible(_s, _t, 0, mid)) ans = mid, r = mid - 1;
      else l = mid + 1;
    }
    return ans;
  }
};

int n, m;
string a[N];

int check(int d) {
  LR_Flow flow(n + m + 2);
  int s = n+m, t = n+m+1;
  // n hang
  for (int i = 0; i < n; ++i) {
    int cnt0 = 0, cnt1 = 0;
    for (int j = 0; j < m; ++j)
      if (a[i][j] == '0') cnt0++;
      else if (a[i][j] == '1') cnt1++;
    // -d <= m-2cnt1-2x <= d
    // (m-2cnt1-d+1)/2 <= x <= (m-2cnt1+d)/2
    if (m-2*cnt1+d < 0) return 0;
    int l = max(0, (m-2*cnt1-d+1)/2), r = min(m-cnt0-cnt1, (m-2*cnt1+d)/2);
    if (l > r) return 0;
    flow.add_edge(s, i, l, r);
  }
  // m cot
  for (int i = 0; i < m; ++i) {
    int cnt0 = 0, cnt1 = 0;
    for (int j = 0; j < n; ++j)
      if (a[j][i] == '0') cnt0++;
      else if (a[j][i] == '1') cnt1++;
    if (n-2*cnt1+d < 0) return 0;
    int l = max(0, (n-2*cnt1-d+1)/2), r = min(n-cnt0-cnt1, (n-2*cnt1+d)/2);
    if (l > r) return 0;
    flow.add_edge(n+i, t, l, r);
  }
  for (int i = 0; i < n; ++i) for (int j = 0; j < m; ++j)
    if (a[i][j] == '*') flow.add_edge(i, n+j, 0, 1);
    //else flow.add_edge(i, n+j, 0, 0);
  return flow.max_flow(s, t) != -1;
}

int main() {
  cin.tie(0)->sync_with_stdio(0); cout.tie(0);
  if (fopen("star.inp", "r")) 
    freopen("star.inp", "r", stdin),
    freopen("star.out", "w", stdout);
  cin >> n >> m;
  for (int i = 0; i < n; ++i)
    cin >> a[i];
  
  int lo = 0, hi = max(n, m), mid, ans = -1;
  while (lo <= hi) {
    mid = lo + hi >> 1;
    if (check(mid))
      ans = mid, hi = mid - 1;
    else 
      lo = mid + 1;
  }
  cout << ans;
	return 0;
}

