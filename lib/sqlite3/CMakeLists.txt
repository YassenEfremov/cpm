cmake_minimum_required(VERSION 3.1.0)
project(sqlite3)

add_library(sqlite3 STATIC sqlite3.c)
#set_target_properties(sqlite PROPERTIES OUTPUT_NAME sqlite3)

# add_definitions(-DSQLITE_ENABLE_RTREE)
# add_definitions(-DSQLITE_ENABLE_FTS4)
# add_definitions(-DSQLITE_ENABLE_FTS5)
# add_definitions(-DSQLITE_ENABLE_JSON1)
# add_definitions(-DSQLITE_ENABLE_RBU)
# add_definitions(-DSQLITE_ENABLE_STAT4)
