#include <system/dbsystem.hpp>

int main() {
    dbsystem * mydb = new dbsystem();
    mydb->createDatabase("test");
    mydb->useDatabase("test");
    return 0;
}