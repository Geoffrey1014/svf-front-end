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


        Expected dominators:
        "0" -> {0}
        "1" -> {0}
        "2" -> {1}
        "3" -> {1}
        "4" -> {3}
        "5" -> {1}
        "6" -> {5}
        "7" -> {5}
        "8" -> {5}
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
};



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
    
    // Test immediate dominators for each block
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

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}