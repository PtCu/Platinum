#include <glog/logging.h>
#include <gtest/gtest.h>


int main(int argc, char**argv){
    google::InitGoogleLogging(argv[0]);
    FLAGS_stderrthreshold = 1;
    printf("Running main() from test/main.cpp\n");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}