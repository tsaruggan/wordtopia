#include "database.h"

int main() {
    Database db("dictionary.db");
    db.populate("../dictionary", "dictionary");
    db.close();
    return 0;
}