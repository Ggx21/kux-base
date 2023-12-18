#include <system/dbsystem.hpp>

int main() {
    dbsystem * mydb = new dbsystem();
    mydb->createDatabase("test");
    return 0;
}