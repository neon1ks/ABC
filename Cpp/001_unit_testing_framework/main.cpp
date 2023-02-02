#include <iostream>
#include <sstream>

#include <exception>
#include <stdexcept>

#include <map>
#include <set>
#include <string>
#include <utility> // for pair
#include <vector>

using namespace std;

using Synonyms = map<string, set<string>>;

void addSynonyms(Synonyms &synonyms, const string &first_word, const string &second_word)
{
    synonyms[second_word].insert(first_word);
    synonyms[first_word].insert(second_word);
}

size_t getSynonymCount(const Synonyms &synonyms, const string &word)
{
    const auto it = synonyms.find(word);
    if (it != synonyms.cend()) {
        return it->second.size();
    }
    return 0;
}

bool areSynonyms(const Synonyms &synonyms, const string &first_word, const string &second_word)
{
    if (synonyms.find(first_word) != synonyms.end()) {
        return synonyms.at(first_word).find(second_word) != synonyms.at(first_word).end();
    }
    return false;
}

namespace neon1ks {

//=======================
// Out
//=======================

template<typename First, typename Second>
ostream &operator<<(ostream &out, const pair<First, Second> &p);

template<typename T>
ostream &operator<<(ostream &out, const vector<T> &v);

template<typename T>
ostream &operator<<(ostream &out, const set<T> &s);

template<typename Key, typename Value>
ostream &operator<<(ostream &out, const map<Key, Value> &m);

template<typename Collection, typename D>
string Join(const Collection &c, const D &d)
{
    stringstream ss;
    bool first = true;
    for (const auto &i : c) {
        if (first) {
            first = false;
        } else {
            ss << d;
        }
        ss << i;
    }
    return ss.str();
}

template<typename First, typename Second>
ostream &operator<<(ostream &out, const pair<First, Second> &p)
{
    return out << '(' << p.first << ", " << p.second << ')';
}

template<typename T>
ostream &operator<<(ostream &out, const vector<T> &v)
{
    return out << '[' << Join(v, ", ") << ']';
}

template<typename T>
std::ostream &operator<<(std::ostream &out, const std::set<T> &s)
{
    return out << '{' << Join(s, ", ") << '}';
}
template<typename Key, typename Value>
ostream &operator<<(ostream &out, const map<Key, Value> &m)
{
    out << '{';
    bool first = true;
    for (const auto &kv : m) {
        if (!first) {
            out << ", ";
        }
        first = false;
        out << kv.first << ": " << kv.second;
    }
    return out << '}';
}

//=======================
// Testing
//=======================

template<typename T, typename U>
void AssertEqual(const T &t, const U &u, const string &hint = {})
{
    if (t != u) {
        ostringstream os;
        os << "Assertion failed: " << t << " != " << u << " Hint: " << hint;
        throw runtime_error(os.str());
    }
}

void Assert(bool b, const string &hint = {})
{
    AssertEqual(b, true, hint);
}

void TestAddSynonyms()
{
    {
        Synonyms synonyms;
        addSynonyms(synonyms, "a", "b");
        // clang-format off
        const Synonyms expected = {
            { "a", { "b" } },
            { "b", { "a" } }
        };
        // clang-format on
        AssertEqual(synonyms, expected);
    }
    {
        // clang-format off
        Synonyms synonyms = {
            { "a", { "b" } },
            { "b", { "a", "c" } },
            { "c", { "b" } }
        };
        // clang-format on
        addSynonyms(synonyms, "a", "c");
        // clang-format off
        const Synonyms expected = {
            { "a", { "b", "c" } },
            { "b", { "a", "c" } },
            { "c", { "a", "b" } }
        };
        // clang-format on
        AssertEqual(synonyms, expected);
    }
}

void TestCount()
{
    {
        const Synonyms empty;
        AssertEqual(getSynonymCount(empty, "a"), 0U, "count for empty");
    }
    {
        // clang-format off
        const Synonyms synonyms = {
            { "a", { "b", "c" } },
            { "b", { "a" } },
            { "c", { "a" } }
        };
        // clang-format on
        AssertEqual(getSynonymCount(synonyms, "a"), 2U, "count for a");
        AssertEqual(getSynonymCount(synonyms, "b"), 1U, "count for b");
        AssertEqual(getSynonymCount(synonyms, "z"), 0U, "count for z");
    }
}

void TestAreSynonyms()
{
    {
        const Synonyms empty;
        Assert(!areSynonyms(empty, "a", "b"), "Empty a b");
        Assert(!areSynonyms(empty, "b", "a"), "Empty b a");
    }

    // clang-format off
    {
        const Synonyms synonyms = {
            { "a", { "b", "c" } },
            { "b", { "a" } },
            { "c", { "a" } }
        };
        // clang-format on

        Assert(areSynonyms(synonyms, "a", "b"));
        Assert(areSynonyms(synonyms, "b", "a"));
        Assert(areSynonyms(synonyms, "a", "c"));
        Assert(areSynonyms(synonyms, "c", "a"));
        Assert(!areSynonyms(synonyms, "b", "c"));
        Assert(!areSynonyms(synonyms, "c", "b"));
    }
}

class TestRunner
{
public:
    TestRunner() = default;
    TestRunner(const TestRunner &) = delete;
    TestRunner &operator=(const TestRunner &) = delete;
    TestRunner(TestRunner &&) = delete;
    TestRunner &operator=(TestRunner &&) = delete;

    template<typename TestFunc>
    void RunTest(TestFunc func, const string &test_name)
    {
        try {
            func();
            cerr << test_name << " OK" << endl;
        } catch (runtime_error &e) {
            ++fail_count;
            cerr << test_name << " fail: " << e.what() << endl;
        } catch (...) {
            ++fail_count;
            cerr << "Unknown exception caught" << endl;
        }
    }
    ~TestRunner()
    {
        if (fail_count > 0) {
            cerr << fail_count << " tests failed. Terminate" << endl;
            exit(1);
        }
    }

private:
    int fail_count = 0;
};

void TestAll()
{
    TestRunner tr;
    // Ссылка на функцию с тестами и название теста
    tr.RunTest(TestAreSynonyms, "TestAreSynonyms");
    tr.RunTest(TestAddSynonyms, "TestAddSynonyms");
    tr.RunTest(TestCount, "TestCount");
}

} // namespace neon1ks

int readInt()
{
    int q{};
    cin >> q;
    return q;
}

int main()
{
#ifdef RUN_TESTS
    neon1ks::TestAll();
#endif

    Synonyms synonyms;

    const int q = readInt();
    for (int i = 0; i < q; ++i) {
        string operation_code;
        cin >> operation_code;

        if (operation_code == "ADD"s) {
            string first_word;
            string second_word;
            cin >> first_word >> second_word;
            addSynonyms(synonyms, first_word, second_word);
        } else if (operation_code == "COUNT") {
            string word;
            cin >> word;
            cout << getSynonymCount(synonyms, word) << endl;
        } else if (operation_code == "CHECK") {
            string first_word;
            string second_word;
            cin >> first_word >> second_word;
            if (areSynonyms(synonyms, first_word, second_word)) {
                cout << "YES" << endl;
            } else {
                cout << "NO" << endl;
            }
        }
    }

    return 0;
}
