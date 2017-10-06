/**
 * The Example on http://blog.csdn.net/itplus/article/details/9361915
 * A great explanation of Q-learning
 */
#include <bits/stdc++.h>
using namespace std;

int R[6][6] = {
    -1, -1, -1, -1, 0, -1,
    -1, -1, -1, 0, -1, 100,
    -1, -1, -1, 0, -1, -1,
    -1, 0, 0, -1, 0, -1,
    0, -1, -1, 0, -1, 100,
    -1, 0, -1, -1, 0, 100
};
double Q[6][6] = {0};
int iter_od[6];
void shu(void) { for (int i=0; i<6; ++i) swap(iter_od[i], iter_od[rand()%6]); } // shuffle the index order

int main(int argc, char **argv) {
    if (argc<3) return 1;
    if (argc<4)
        srand(time(NULL));
    else
        srand(atoi(argv[3]));
    for (int i=0; i<6; ++i) iter_od[i]=i;
    int train_iter = atoi(argv[1]);
    for (int e=1; e<=train_iter; ++e) { // episode
        int crr_s = rand()%6; // randomly select an initial state
        cout << "Iteration " << e << ": " << endl;
        cout << "    " << crr_s;
        for (int t=2; crr_s!=5; ++t) { // if not reach the goal
            int n=0;
            for (int i=0; i<6; ++i) n+=(R[crr_s][i]>=0);
            int act_n = rand()%n; // randomly select a legal action
            int act_i;
            n=0;
            for (int i=0; i<6; ++i) {
                if (R[crr_s][i]>=0) {
                    if (n==act_n) {
                        act_i = i; // select an action, and we get s'
                        break;
                    }
                    ++n;
                }
            }
            double maxv=0;
            for (int i=0; i<6; ++i) {
                if (R[act_i][i]>=0) {
                    maxv = max(maxv, Q[act_i][i]); // max_{a'}{Q(s',a')} from (1.1)
                }
            }
            Q[crr_s][act_i] = R[crr_s][act_i] + 0.8*maxv; // from formula (1.1) ~ Q(s,a) := R(s,a) + r*max_{a'}{Q(s',a')}
            crr_s = act_i; // transition to new status
            cout << " -> " << crr_s;
            if (t%4==0) cout << endl;
        }
        cout << '\n' << endl;
    }
    double norm = 1.;
    cout << "The Q Matrix:\n" << endl;
    for (int i=0; i<6; ++i) 
        for (int j=0; j<6; ++j) 
            norm = max(norm, Q[i][j]);
    for (int i=0; i<6; ++i) {
        for (int j=0; j<6; ++j) {
            Q[i][j]/=norm; // normalization
            cout << setw(5) << fixed << setprecision(1) << Q[i][j];
        }
        cout << endl;
    }
    cout << endl;
    int crr_s = atoi(argv[2]);
    cout << "    " << crr_s;
    int t = 1;
    while(crr_s!=5) { // find goal -> 5
        double maxv=-1e9;
        int a=0;
        shu(); // do some shuffle to prevent infinite loop
        for (int i=0; i<6; ++i) { 
            if (R[crr_s][iter_od[i]]>=0) { // find a legal action
                if (Q[crr_s][iter_od[i]]>maxv) { // with maximum value in Q
                    a = iter_od[i];
                    maxv = Q[crr_s][iter_od[i]];
                }
            }
        }
        crr_s = a; // transition to new status
        cout << " -> " << crr_s;
        ++t;
        if (t%4==0) cout << endl;
    }
    cout << endl;
    cout << "Total distnace: " << t-1 << endl;
    return 0;
}
