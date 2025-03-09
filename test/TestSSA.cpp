#include <gtest/gtest.h>
#include "CFG.h"
#include "LlBuilder.h"

class TestSSA : public ::testing::Test {
protected:
    CFG* createTestCFG1() {
        /*
        Create this CFG:
            A
           / \
          B   C
           \ /
            D
            |
            E

        Expected immediate dominators:
        A -> {A}
        B -> {A}
        C -> {A}
        D -> {A}
        E -> {D}
        */
        
        CFG* cfg = new CFG();
        
        // Create blocks
        BasicBlock* blockA = new BasicBlock("A");
        BasicBlock* blockB = new BasicBlock("B");
        BasicBlock* blockC = new BasicBlock("C");
        BasicBlock* blockD = new BasicBlock("D");
        BasicBlock* blockE = new BasicBlock("E");
        
        // Add blocks to CFG
        cfg->addBlock(blockA);
        cfg->addBlock(blockB);
        cfg->addBlock(blockC);
        cfg->addBlock(blockD);
        cfg->addBlock(blockE);
        
        // Set entry
        cfg->setEntry(blockA);
        
        // Connect blocks
        blockA->addSuccessor(blockB);
        blockA->addSuccessor(blockC);
        blockB->addPredecessor(blockA);
        blockC->addPredecessor(blockA);
        
        blockB->addSuccessor(blockD);
        blockC->addSuccessor(blockD);
        blockD->addPredecessor(blockB);
        blockD->addPredecessor(blockC);
        
        blockD->addSuccessor(blockE);
        blockE->addPredecessor(blockD);
        
        return cfg;
    }

    CFG* createTestCFG2() {

        /*
        Create this CFG:
           "6" -> "7";
            "7" -> "3";
            "5" -> "8";
            "5" -> "6";
            "8" -> "7";
            "3" -> "4";
            "2" -> "3";
            "1" -> "5";
            "1" -> "2";
            "0" -> "1";
        */

        CFG* cfg = new CFG();
        BasicBlock* block0 = new BasicBlock("0");
        BasicBlock* block1 = new BasicBlock("1");
        BasicBlock* block2 = new BasicBlock("2");
        BasicBlock* block3 = new BasicBlock("3");
        BasicBlock* block4 = new BasicBlock("4");
        BasicBlock* block5 = new BasicBlock("5");
        BasicBlock* block6 = new BasicBlock("6");
        BasicBlock* block7 = new BasicBlock("7");
        BasicBlock* block8 = new BasicBlock("8");
        
        cfg->addBlock(block0);
        cfg->addBlock(block1);
        cfg->addBlock(block2);
        cfg->addBlock(block3);
        cfg->addBlock(block4);
        cfg->addBlock(block5);
        cfg->addBlock(block6);
        cfg->addBlock(block7);
        cfg->addBlock(block8);

        cfg->setEntry(block0);

        block0->addSuccessor(block1);
        block1->addPredecessor(block0);

        block1->addSuccessor(block2);
        block2->addPredecessor(block1);

        block2->addSuccessor(block3);
        block3->addPredecessor(block2);

        block3->addSuccessor(block4);
        block4->addPredecessor(block3);

        block3->addSuccessor(block1);
        block1->addPredecessor(block3);

        block1->addSuccessor(block5);
        block5->addPredecessor(block1);

        block5->addSuccessor(block6);
        block6->addPredecessor(block5);
        block5->addSuccessor(block8);
        block8->addPredecessor(block5);

        block6->addSuccessor(block7);
        block7->addPredecessor(block6);

        block8->addSuccessor(block7);
        block7->addPredecessor(block8);

        block7->addSuccessor(block3);
        block3->addPredecessor(block7);
    
        

        cfg->writeDotFile("cfg_test2.dot");
        
        return cfg;
    }
    
    CFG* createTestCFG3() {
        /*
        Create this CFG:
        6 -> 5;
        6 -> 4;
        5 -> 1;
        4 -> 2;
        4 -> 3;
        1 -> 2;
        2 -> 1;
        3 -> 2;
        2 -> 3;
        */
        CFG* cfg = new CFG();
        BasicBlock* block6 = new BasicBlock("6");
        BasicBlock* block5 = new BasicBlock("5");
        BasicBlock* block4 = new BasicBlock("4");
        BasicBlock* block3 = new BasicBlock("3");
        BasicBlock* block2 = new BasicBlock("2");
        BasicBlock* block1 = new BasicBlock("1");

        cfg->addBlock(block6);
        cfg->addBlock(block5);
        cfg->addBlock(block4);
        cfg->addBlock(block3);
        cfg->addBlock(block2);
        cfg->addBlock(block1);

        cfg->setEntry(block6);

        block6->addSuccessor(block5);
        block6->addSuccessor(block4);

        block5->addPredecessor(block6);
        block4->addPredecessor(block6);

        block5->addSuccessor(block1);
        block1->addPredecessor(block5);

        block4->addSuccessor(block2);
        block2->addPredecessor(block4);

        block4->addSuccessor(block3);
        block3->addPredecessor(block4);

        block2->addSuccessor(block3);
        block3->addPredecessor(block2);

        block3->addSuccessor(block2);
        block2->addPredecessor(block3);

        block2->addSuccessor(block1);
        block1->addPredecessor(block2);

        block1->addSuccessor(block2);
        block2->addPredecessor(block1);

        cfg->writeDotFile("cfg_test3.dot");

        return cfg;

    }

    CFG* createTestCFG4() {
        /*
        Create this CFG:
        0 -> 1;
        1 -> 2;
        2 -> 1;
        1 -> 3;
        3 -> 4;
        */

        CFG* cfg = new CFG();
        BasicBlock* block0 = new BasicBlock("0");
        BasicBlock* block1 = new BasicBlock("1");
        BasicBlock* block2 = new BasicBlock("2");
        BasicBlock* block3 = new BasicBlock("3");
        BasicBlock* block4 = new BasicBlock("4");

        cfg->addBlock(block0);
        cfg->addBlock(block1);
        cfg->addBlock(block2);
        cfg->addBlock(block3);
        cfg->addBlock(block4);

        cfg->setEntry(block0);

        block0->addSuccessor(block1);
        block1->addPredecessor(block0);

        block1->addSuccessor(block2);
        block2->addPredecessor(block1);

        block2->addSuccessor(block1);
        block1->addPredecessor(block2);

        block1->addSuccessor(block3);
        block3->addPredecessor(block1);

        block3->addSuccessor(block4);
        block4->addPredecessor(block3);

        cfg->writeDotFile("cfg_test4.dot");

        return cfg;        
    }
};

    CFG* createTestCFG5() {
        /*
        Create this CFG:
        entry -> 1;
        entry -> 2;
        1 -> 5;
        2 -> 3;
        2 -> 4;
        3 -> 6;
        4 -> 6;
        5 -> 1;
        5 -> 7;
        6 -> 2;
        6 -> 7;
        7 -> 8;
        7 -> 9;
        8 -> 10;
        8 -> 8;
        9 -> 10;
        10 -> exit;
        10 -> 7;
       */

        CFG* cfg = new CFG();
        BasicBlock* entry = new BasicBlock("entry");
        BasicBlock* block1 = new BasicBlock("1");
        BasicBlock* block2 = new BasicBlock("2");
        BasicBlock* block3 = new BasicBlock("3");
        BasicBlock* block4 = new BasicBlock("4");
        BasicBlock* block5 = new BasicBlock("5");
        BasicBlock* block6 = new BasicBlock("6");
        BasicBlock* block7 = new BasicBlock("7");
        BasicBlock* block8 = new BasicBlock("8");
        BasicBlock* block9 = new BasicBlock("9");
        BasicBlock* block10 = new BasicBlock("10");
        BasicBlock* exit = new BasicBlock("exit");

        cfg->addBlock(entry);
        cfg->addBlock(block1);
        cfg->addBlock(block2);
        cfg->addBlock(block3);
        cfg->addBlock(block4);
        cfg->addBlock(block5);
        cfg->addBlock(block6);
        cfg->addBlock(block7);
        cfg->addBlock(block8);
        cfg->addBlock(block9);
        cfg->addBlock(block10);
        cfg->addBlock(exit);

        cfg->setEntry(entry);

        entry->addSuccessor(block1);
        entry->addSuccessor(block2);

        block1->addPredecessor(entry);
        block2->addPredecessor(entry);

        block1->addSuccessor(block5);
        block5->addPredecessor(block1);

        block2->addSuccessor(block3);
        block3->addPredecessor(block2);

        block2->addSuccessor(block4);
        block4->addPredecessor(block2);

        block3->addSuccessor(block6);
        block6->addPredecessor(block3);

        block4->addSuccessor(block6);
        block6->addPredecessor(block4);

        block5->addSuccessor(block1);
        block1->addPredecessor(block5);

        block5->addSuccessor(block7);
        block7->addPredecessor(block5);

        block6->addSuccessor(block2);
        block2->addPredecessor(block6);

        block6->addSuccessor(block7);
        block7->addPredecessor(block6);

        block7->addSuccessor(block8);
        block8->addPredecessor(block7);

        block7->addSuccessor(block9);
        block9->addPredecessor(block7);

        block8->addSuccessor(block10);
        block10->addPredecessor(block8);

        block8->addSuccessor(block8);
        block8->addPredecessor(block8);

        block9->addSuccessor(block10);
        block10->addPredecessor(block9);

        block10->addSuccessor(exit);
        exit->addPredecessor(block10);  

        block10->addSuccessor(block7);
        block7->addPredecessor(block10);

        cfg->writeDotFile("cfg_test5.dot");

        return cfg;
        
    }


TEST_F(TestSSA, TestDominators1) {
    CFG* cfg = createTestCFG1();
    SSAGenerator ssaGen;
    
    // Make computeDominators accessible for testing
    class TestSSAGenerator : public SSAGenerator {
    public:
        using SSAGenerator::computeDominators;
        using SSAGenerator::getIdoms;
    };
    
    TestSSAGenerator testGen;
    testGen.computeDominators(cfg);
    
    
    // Get blocks by label
    BasicBlock* A = cfg->getBlock("A");
    BasicBlock* B = cfg->getBlock("B");
    BasicBlock* C = cfg->getBlock("C");
    BasicBlock* D = cfg->getBlock("D");
    BasicBlock* E = cfg->getBlock("E");
    
    // Test immediate dominators for each block
    const auto& idoms = testGen.getIdoms();
    EXPECT_EQ(idoms.at(A), A);
    EXPECT_EQ(idoms.at(B), A);
    EXPECT_EQ(idoms.at(C), A);
    EXPECT_EQ(idoms.at(D), A);
    EXPECT_EQ(idoms.at(E), D);
    
    
    delete cfg;

} 

TEST_F(TestSSA, TestDominators2) {
    CFG* cfg = createTestCFG2();
    SSAGenerator ssaGen;
    ssaGen.computeDominators(cfg);

    // Expected dominators:
    //     "0" -> {0}
    //     "1" -> {0}
    //     "2" -> {1}
    //     "3" -> {1}
    //     "4" -> {3}
    //     "5" -> {1}
    //     "6" -> {5}
    //     "7" -> {5}
    //     "8" -> {5}
       

    // Get blocks by label
    BasicBlock* block0 = cfg->getBlock("0");
    BasicBlock* block1 = cfg->getBlock("1");
    BasicBlock* block2 = cfg->getBlock("2");
    BasicBlock* block3 = cfg->getBlock("3");
    BasicBlock* block4 = cfg->getBlock("4");
    BasicBlock* block5 = cfg->getBlock("5");
    BasicBlock* block6 = cfg->getBlock("6");
    BasicBlock* block7 = cfg->getBlock("7");
    BasicBlock* block8 = cfg->getBlock("8");
    
    const auto& idoms = ssaGen.getIdoms();
    EXPECT_EQ(idoms.at(block0), block0);
    EXPECT_EQ(idoms.at(block1), block0);
    EXPECT_EQ(idoms.at(block2), block1);
    EXPECT_EQ(idoms.at(block3), block1);
    EXPECT_EQ(idoms.at(block4), block3);
    EXPECT_EQ(idoms.at(block5), block1);
    EXPECT_EQ(idoms.at(block6), block5);
    EXPECT_EQ(idoms.at(block7), block5);
    EXPECT_EQ(idoms.at(block8), block5);

//    ssaGen.computeDominanceFrontier(cfg);
//    ssaGen.printDominanceFrontier();

    delete cfg;
    
}

TEST_F(TestSSA, TestDominators3) {
    CFG* cfg = createTestCFG3();
    SSAGenerator ssaGen;
    ssaGen.computeDominators(cfg);

    // Get blocks by label
    BasicBlock* block6 = cfg->getBlock("6");
    BasicBlock* block5 = cfg->getBlock("5");
    BasicBlock* block4 = cfg->getBlock("4");
    BasicBlock* block3 = cfg->getBlock("3");
    BasicBlock* block2 = cfg->getBlock("2");
    BasicBlock* block1 = cfg->getBlock("1");

    // Test immediate dominators for each block
    // Expected dominators:
    //     "6" -> {6}
    //     "5" -> {6}
    //     "4" -> {6}
    //     "3" -> {6}
    //     "2" -> {6}
    //     "1" -> {6}

    const auto& idoms = ssaGen.getIdoms();
    EXPECT_EQ(idoms.at(block6), block6);
    EXPECT_EQ(idoms.at(block5), block6);
    EXPECT_EQ(idoms.at(block4), block6);
    EXPECT_EQ(idoms.at(block3), block6);
    EXPECT_EQ(idoms.at(block2), block6);
    EXPECT_EQ(idoms.at(block1), block6);

    delete cfg;
}

TEST_F(TestSSA, TestDominators4) {
    CFG* cfg = createTestCFG4();
    SSAGenerator ssaGen;
    ssaGen.computeDominators(cfg);

    // Get blocks by label
    BasicBlock* block0 = cfg->getBlock("0");
    BasicBlock* block1 = cfg->getBlock("1");
    BasicBlock* block2 = cfg->getBlock("2");
    BasicBlock* block3 = cfg->getBlock("3");
    BasicBlock* block4 = cfg->getBlock("4");
    
    // Test immediate dominators for each block
    // Expected dominators:
    //     "0" -> {0}
    //     "1" -> {0}
    //     "2" -> {1}
    //     "3" -> {1}
    //     "4" -> {3}

    const auto& idoms = ssaGen.getIdoms();
    EXPECT_EQ(idoms.at(block0), block0);
    EXPECT_EQ(idoms.at(block1), block0);
    EXPECT_EQ(idoms.at(block2), block1);
    EXPECT_EQ(idoms.at(block3), block1);
    EXPECT_EQ(idoms.at(block4), block3);
    

    delete cfg;
}

TEST_F(TestSSA, TestDominators5) {
    CFG* cfg = createTestCFG5();
    SSAGenerator ssaGen;
    ssaGen.computeDominators(cfg);

    // Get blocks by label  
    BasicBlock* entry = cfg->getBlock("entry");
    BasicBlock* block1 = cfg->getBlock("1");
    BasicBlock* block2 = cfg->getBlock("2");
    BasicBlock* block3 = cfg->getBlock("3");
    BasicBlock* block4 = cfg->getBlock("4");
    BasicBlock* block5 = cfg->getBlock("5");
    BasicBlock* block6 = cfg->getBlock("6");
    BasicBlock* block7 = cfg->getBlock("7");
    BasicBlock* block8 = cfg->getBlock("8");
    BasicBlock* block9 = cfg->getBlock("9");
    BasicBlock* block10 = cfg->getBlock("10");
    BasicBlock* exit = cfg->getBlock("exit");   

    // Test immediate dominators for each block
    // Expected dominators:
    //     "entry" -> {entry}
    //     "1" -> {entry}
    //     "2" -> {entry}
    //     "3" -> {2}
    //     "4" -> {2}
    //     "5" -> {1}
    //     "6" -> {2}
    //     "7" -> {entry}
    //     "8" -> {7}
    //     "9" -> {7}
    //     "10" -> {7}
    //     "exit" -> {10}

    const auto& idoms = ssaGen.getIdoms();
    EXPECT_EQ(idoms.at(entry), entry);
    EXPECT_EQ(idoms.at(block1), entry);
    EXPECT_EQ(idoms.at(block2), entry);
    EXPECT_EQ(idoms.at(block3), block2);
    EXPECT_EQ(idoms.at(block4), block2);
    EXPECT_EQ(idoms.at(block5), block1);
    EXPECT_EQ(idoms.at(block6), block2);
    EXPECT_EQ(idoms.at(block7), entry);
    EXPECT_EQ(idoms.at(block8), block7);
    EXPECT_EQ(idoms.at(block9), block7);
    EXPECT_EQ(idoms.at(block10), block7);
    EXPECT_EQ(idoms.at(exit), block10);

    // Test dominance frontier for each block
    ssaGen.computeDominanceFrontier(cfg);
    const auto& df = ssaGen.getDominanceFrontier();

    EXPECT_EQ(df.at(entry), std::unordered_set<BasicBlock*>());
    EXPECT_EQ(df.at(block1), std::unordered_set<BasicBlock*>({block1,block7}));
    EXPECT_EQ(df.at(block2), std::unordered_set<BasicBlock*>({block2, block7}));
    EXPECT_EQ(df.at(block3), std::unordered_set<BasicBlock*>({block6}));
    EXPECT_EQ(df.at(block4), std::unordered_set<BasicBlock*>({block6}));
    EXPECT_EQ(df.at(block5), std::unordered_set<BasicBlock*>({block1, block7}));
    EXPECT_EQ(df.at(block6), std::unordered_set<BasicBlock*>({block2, block7}));
    EXPECT_EQ(df.at(block7), std::unordered_set<BasicBlock*>({block7}));
    EXPECT_EQ(df.at(block8), std::unordered_set<BasicBlock*>({block8, block10}));
    EXPECT_EQ(df.at(block9), std::unordered_set<BasicBlock*>({block10}));
    EXPECT_EQ(df.at(block10), std::unordered_set<BasicBlock*>({block7}));
    EXPECT_EQ(df.at(exit), std::unordered_set<BasicBlock*>());
    


    delete cfg;
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}