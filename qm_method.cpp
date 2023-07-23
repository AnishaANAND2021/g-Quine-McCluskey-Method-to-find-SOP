
#include <bits/stdc++.h>
using namespace std;

set<int> sd;                             // for checking useless don't cares in prime implicants
vector<int> arr_min, arr_max, dont_care; // inputs
const int N = 1e5;
vector<int> bucket[N];                                   // a graph that stores element on the basis of no of 1's in its binary
vector<pair<int, pair<int, int>>> bucket_s[N];           // stores element with a pair of 2 such that the difference is a power of 2
vector<int> implicants;                                  // stores element that can't be grouped
vector<pair<vector<int>, vector<int>>> prime_implicants; // pair. first will contain min terms, pair.second contain differences

// Step 1
void buckets(vector<int> arr)
{
    for (int i = 0; i < arr.size(); i++)
    {
        int x = arr[i];
        int one = 0;
        // Step 2
        while (x)
        {
            if (x % 2)
                one++;
            x /= 2;
        }
        bucket[one].push_back(arr[i]);
    }
}

// Step:3
void making_implicants(int m)
{
    set<int> f;
    for (int i = 0; i <= m - 1; i++)
        for (auto it : bucket[i])
        {
            int x = it;
            set<int> s;
            s.insert(x);

            for (auto itt : bucket[i + 1])
            {
                int y = abs(itt - x);
                if ((itt > x) && (y && (!(y & (y - 1)))))
                {
                    if (m == 2 || m == 4)
                    {
                        s.insert(itt);
                        f.insert(itt);
                        f.insert(x);
                    }
                    if (m == 3)
                    {
                        if (y == 1 && (itt == 4)) // || x == 6 || itt == 1 || x == 1))
                        {
                            continue;
                        }
                        s.insert(itt);
                        f.insert(itt);
                        f.insert(x);
                    }
                }

                if (s.size() == 2)
                {
                    bucket_s[i].push_back({abs(x - itt), {x, itt}});
                    s.erase(itt);
                }
            }
        }

    for (int i = 0; i <= m - 1; i++)
        for (auto it : bucket[i])
        {
            if (f.find(it) == f.end())
                implicants.push_back(it);
        }
}

// Step 4:
void make_prime_implicants(int m)
{
    set<int> f;
    for (int i = 0; i <= m - 1; i++)
        for (auto it : bucket_s[i])
        {
            int x = it.first;
            for (auto itt : bucket_s[i + 1])
            {
                int y = abs(itt.first - x);
                int a = it.second.first, b = it.second.second, c = itt.second.first, d = itt.second.second;
                int e = abs(a - b), g = abs(b - d);
                if (y == 0 && (e && (!(e & (e - 1)))) && (g && (!(g & (g - 1)))))
                    if (f.find(a) == f.end() || f.find(b) == f.end() || f.find(c) == f.end() || f.find(d) == f.end())
                    {
                        f.insert(a);
                        f.insert(b);
                        f.insert(c);
                        f.insert(d);
                        vector<int> min, dif;
                        min.push_back(it.second.first);
                        min.push_back(it.second.second);
                        min.push_back(itt.second.first);
                        min.push_back(itt.second.second);
                        dif.push_back(abs(a - b));
                        dif.push_back(abs(b - d));
                        prime_implicants.push_back({min, dif});
                    }
            }
        }

    for (int i = 0; i <= m - 1; i++)
    {
        for (auto it : bucket_s[i])
        {
            if ((f.find(it.second.first) == f.end()) || (f.find(it.second.second) == f.end()))
            {
                f.insert(it.second.first);
                f.insert(it.second.second);
                vector<int> min, dif;
                min.push_back(it.second.first);
                min.push_back(it.second.second);
                dif.push_back(it.first);
                prime_implicants.push_back({min, dif});
            }
        }
    }
    int iterator = 0;

    // removing don't cares only

    for (auto it : prime_implicants)
    {
        int x = it.first.size();
        int y = 0;
        for (auto i : it.first)
        {
            auto itt = find(dont_care.begin(), dont_care.end(), i);
            if (itt != dont_care.end())
                y++;
        }
        if (x == y && x != 0)
            prime_implicants.erase(prime_implicants.begin() + iterator);
        if (y == 0)
        {
            for (auto i : it.first)
                sd.insert(i);
        }

        iterator++;
    }
}

void print_minterms(int m)
{
    vector<vector<int>> s;
    vector<int> st(m, 0);
    for (auto it : prime_implicants)
    {
        int x = it.first.size();
        int y = 0;
        for (auto i : it.first)
        {
            auto itt = find(dont_care.begin(), dont_care.end(), i);
            if (itt != dont_care.end())
                y++;
        }
        if (y > 0)
        {
            int z = 0;
            for (auto i : it.first)
            {
                auto itt = find(dont_care.begin(), dont_care.end(), i);
                if (itt == dont_care.end() && sd.find(i) != sd.end())
                    z++;
            }
            if (z + y == x)
                continue;
        }
        for (auto i : it.first)
        {
            bitset<32> b = i;
            for (int i = 0; i < m; i++)
            {
                st[i] = st[i] || b[m - 1 - i];
            }
        }
        for (auto i : it.second)
        {
            int x = log2(i);
            st[m - 1 - x] = -1;
        }
        s.push_back(st);
        for (int i = 0; i < m; i++)
        {
            st[i] = 0;
        }
    }
    for (int i = 0; i < implicants.size(); i++)
    {
        auto it = find(dont_care.begin(), dont_care.end(), implicants[i]);
        if (it != dont_care.end())
            continue;
        bitset<32> b = implicants[i];
        for (int i = 0; i < m; i++)
        {
            st[i] = st[i] || b[m - 1 - i];
        }

        s.push_back(st);
        for (int i = 0; i < m; i++)
        {
            st[i] = 0;
        }
    }
    cout << "\nThe SOP expression is:\n   f(";
    char a = 'A';
    for (int i = 0; i < m; i++)
    {
        char b = a + i;
        cout << b;
        if (i < m - 1)
            cout << ',';
    }
    cout << ") = ";
    int x = s.size();
    for (int i = 0; i < s.size(); i++)
    {
        for (int j = 0; j < st.size(); j++)
        {
            if (s[i][j] == 0)
            {
                char b = a + j;
                cout << b << "'";
            }
            if (s[i][j] == 1)
            {
                char b = a + j;
                cout << b;
            }
        }
        x--;
        if (x)
            cout << " + ";
    }
}

int main()
{
    int option, m;
    cout << "Enter '1' if you want to give minterms \nEnter '0' if you want to give maxterms\n";
    cin >> option;
    if (option)
    {
        cout << "Enter the size of mintems : ";
        int size;
        cin >> size;
        cout << "Enter the minterms : ";
        for (int i = 0; i < size; i++)
        {
            int x;
            cin >> x;
            arr_min.push_back(x);
        }
        int size_dontcare;
        cout << "Enter the size of don't care terms : ";
        cin >> size_dontcare;
        if (size_dontcare)
        {
            cout << "Enter don't care terms : ";
            for (int i = 0; i < size_dontcare; i++)
            {
                int x;
                cin >> x;
                auto it = find(arr_min.begin(), arr_min.end(), x);
                if (it == arr_min.end())
                    dont_care.push_back(x);
            }
        }
        int x = *max_element(arr_min.begin(), arr_min.end());
        int y;
        if (!size_dontcare)
            y = 0;
        else
            y = *max_element(dont_care.begin(), dont_care.end());
        m = max(x, y);
    }
    else
    {
        cout << "Enter the size of maxtems : ";
        int size;
        cin >> size;
        cout << "Enter the maxterms : ";
        for (int i = 0; i < size; i++)
        {
            int x;
            cin >> x;
            arr_max.push_back(x);
        }
        int size_dontcare;
        cout << "Enter the size of don't care terms : ";
        cin >> size_dontcare;
        if (size_dontcare)
        {
            cout << "Enter don't care terms : ";
            for (int i = 0; i < size_dontcare; i++)
            {
                int x;
                cin >> x;
                auto it = find(arr_max.begin(), arr_max.end(), x);
                if (it == arr_max.end())
                    dont_care.push_back(x);
            }
        }
        int x = *max_element(arr_max.begin(), arr_max.end());
        int y;
        if (!size_dontcare)
            y = 0;
        else
            y = *max_element(dont_care.begin(), dont_care.end());
        m = max(x, y);
    }
    m = log2(m) + 1;
    if (!option)
    {
        for (int i = 0; i < (1 << m); i++)
        {
            auto it = find(arr_max.begin(), arr_max.end(), i);
            auto itt = find(dont_care.begin(), dont_care.end(), i);
            if (it == arr_max.end() && itt == dont_care.end())
                arr_min.push_back(i);
        }
    }
    buckets(arr_min);
    buckets(dont_care);
    making_implicants(m);
    make_prime_implicants(m);
    print_minterms(m);
    cout << endl
         << endl;
    return 0;
}
