#include <iostream>
#include <string>

class SomeClass {
public:
    SomeClass(const std::string &var) : m_var(var) {}
private:
    std::string m_var;
};

int main(int argc, const char *argv[])
{
    using namespace std;
    cout << "TESTING...";
    cout << "1";
    cout << "2";
    cout << "3";
    return 0;
}
