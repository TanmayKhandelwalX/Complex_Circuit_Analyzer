
#include <simplecpp>
#include <vector>
#include <map>
#include <fstream>
using namespace std;

struct Button
{
    int x, y, w, h;
    Button(const char name[], int xx, int yy, int ww, int hh)
    {
        x = xx;
        y = yy;
        w = ww;
        h = hh;
        Rectangle(x, y, w, h).imprint();
        Text(x, y, name).imprint();
    }
    bool in(int click)
    {
        int cx = click / 65536, cy = click % 65536;
        return (abs(cx - x) <= w && abs(cy - y) <= h);
    }
};
struct MathRep
{
    vector<vector<double>> A;
    vector<double> b;
    void addNode()
    {
        A.push_back(vector<double>(A.size(), 0)); // add all 0 row
        for (size_t i = 0; i < A.size(); i++)
            A[i].push_back(0);
        b.push_back(0);
    }
    void addSource(int i, int j, double val)
    {
        b[i] -= val;
        b[j] += val;
        cout << "help" << endl;
    }
    void addConductance(int i, int j, double val)
    {
        A[i][i] += val;
        A[j][j] += val;
        A[i][j] -= val;
        A[j][i] -= val;
    }

    void solve()
    {
        // Solving for x[i] = voltage of node i
        //(Ax) [i] : current leaving node i through cunductances
        // must equal to b[i] = current entering node i through current sources
        // solve Ax=b
        // use row i to eliminate 0s in column j!=i
        // this will yeild x[i] = b[i]/A[i][i]

        for (size_t i = 0; i < A.size() - 1; i++)
        {
            for (size_t j = 0; j < A.size() - 1; j++)
            {
                if (i != j)
                {
                    double multiplier = A[j][i] / A[i][i];
                    for (size_t k = 0; k < A.size() - 1; k++)
                        A[j][k] -= A[i][k] * multiplier;
                    b[j] = b[j] - b[i] * multiplier;
                }
            }
        }
        cout << "printing node potentials" << endl;
        for (size_t i = 0; i < A.size() - 1; i++)
        {
            cout << i << " = " << b[i] / A[i][i] << endl;
        }
        cout << A.size() - 1 << " = " << 0 << endl;
    }
};
struct Node
{
    const double r = 20;
    double x, y;
    Node(int click, int index)
    {
        x = click / 65536;
        y = click % 65536;
        Circle(x, y, r).imprint();
        Text(x, y, index).imprint();
    }
    bool in(int click)
    {
        return pow((x - click / 65536), 2) + pow((y - click % 65536), 2) < pow(r, 2);
    }
};
struct CanvasContent
{
    vector<Node *> Nptr;
    MathRep *mrp;
    CanvasContent(MathRep *mrp0)
    {
        mrp = mrp0;
    }
    void addNode()
    {
        int vclick = getClick();
        Nptr.push_back(new Node(vclick, Nptr.size()));
        mrp->addNode();
    }
    int selectNode()
    {
        int click = getClick();
        for (size_t i = 0; i < Nptr.size(); i++)
        { // check weather a click was inside any node 1 by 1
            if (Nptr[i]->in(click))
                return i;
        }
        return -1;
    }
    void addConductance()
    {
        int i1 = selectNode(), i2 = selectNode();
        if (i1 >= 0 && i2 >= 0)
        {
            double x1 = Nptr[i1]->x, y1 = Nptr[i1]->y,
                   x2 = Nptr[i2]->x, y2 = Nptr[i2]->y;
            Line(x1, y1, x2, y2).imprint();
            cout << "Conductance value = ";
            double g;
            cin >> g;
            Text((x1 + x2) / 2, (y1 + y2) / 2, g).imprint();
            mrp->addConductance(i1, i2, g);
        }
        else
            cout << "select operation again" << endl;
    }
    void addSource()
    {
        int i1 = selectNode(), i2 = selectNode();
        if (i1 >= 0 && i2 >= 0)
        {
            double x1 = Nptr[i1]->x, y1 = Nptr[i1]->y,
                   x2 = Nptr[i2]->x, y2 = Nptr[i2]->y;
            Line(x1, y1, x2, y2).imprint();
            cout << "Current source strength = ";
            double j;
            cin >> j;
            Text((x1 + x2) / 2, (y1 + y2) / 2, j).imprint();
            mrp->addSource(i1, i2, j);
            Turtle t;
            t.penUp();
            t.moveTo((x1 + x2) / 2, (y1 + y2) / 2);
            t.face(x2, y2);
            int times = 10;
            while (times >= 0)
            {
                t.forward(10);
                t.forward(-9.09);
                times--;
            }
            t.imprint();
        }
        else
            cout << "select operation again" << endl;
    }
};

int main()
{
    initCanvas("Circuit editor", 600, 600);
    Button b1("Node", 100, 50, 160, 30);
    Button b2("Conductance", 100, 150, 160, 30);
    Button b3("Source", 100, 200, 160, 30);
    Button b4("Solve", 100, 250, 160, 30);
    Button b5("Quit", 100, 300, 160, 30);

    MathRep mr;
    CanvasContent cc(&mr);

    while (true)
    {
        int cpoint = getClick();
        if (b1.in(cpoint))
            cc.addNode();
        else if (b2.in(cpoint))
            cc.addConductance();
        else if (b3.in(cpoint))
            cc.addSource();
        else if (b4.in(cpoint))
            mr.solve();
        else if (b5.in(cpoint))
            break;
    }
}