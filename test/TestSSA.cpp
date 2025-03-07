#include <gtest/gtest.h>
#include "CFG.h"
#include "LlBuilder.h"

class TestSSA : public ::testing::Test {
protected:
    CFG* createTestCFG() {
        /*
        Create this CFG:
            A
           / \
          B   C
           \ /
            D
            |
            E

        Expected dominators:
        A -> {A}
        B -> {A, B}
        C -> {A, C}
        D -> {A, D}
        E -> {A, D, E}
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
        "1" -> {0, 1}
        "2" -> {0, 1, 2}
        "3" -> {0, 1, 3}
        "4" -> {0, 1, 3, 4}
        "5" -> {0, 1, 5}
        "6" -> {0, 1, 5, 6}
        "7" -> {0, 1, 5, 7}
        "8" -> {0, 1, 5, 8}
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
    
};

TEST_F(TestSSA, TestDominators) {
    CFG* cfg = createTestCFG();
    SSAGenerator ssaGen;
    
    // Make computeDominators accessible for testing
    class TestSSAGenerator : public SSAGenerator {
    public:
        using SSAGenerator::computeDominators;
        using SSAGenerator::getDom;
    };
    
    TestSSAGenerator testGen;
    testGen.computeDominators(cfg);
    const auto& dom = testGen.getDom();
    
    // Get blocks by label
    BasicBlock* A = cfg->getBlock("A");
    BasicBlock* B = cfg->getBlock("B");
    BasicBlock* C = cfg->getBlock("C");
    BasicBlock* D = cfg->getBlock("D");
    BasicBlock* E = cfg->getBlock("E");
    
    // Test dominators for each block
    EXPECT_EQ(dom.at(A).size(), 1);
    EXPECT_TRUE(dom.at(A).count(A) > 0);
    
    EXPECT_EQ(dom.at(B).size(), 2);
    EXPECT_TRUE(dom.at(B).count(A) > 0);
    EXPECT_TRUE(dom.at(B).count(B) > 0);
    
    EXPECT_EQ(dom.at(C).size(), 2);
    EXPECT_TRUE(dom.at(C).count(A) > 0);
    EXPECT_TRUE(dom.at(C).count(C) > 0);
    
    EXPECT_EQ(dom.at(D).size(), 2);
    EXPECT_TRUE(dom.at(D).count(A) > 0);
    EXPECT_TRUE(dom.at(D).count(D) > 0);
    
    EXPECT_EQ(dom.at(E).size(), 3);
    EXPECT_TRUE(dom.at(E).count(A) > 0);
    EXPECT_TRUE(dom.at(E).count(D) > 0);
    EXPECT_TRUE(dom.at(E).count(E) > 0);
    
    delete cfg;

} 

TEST_F(TestSSA, TestDominators2) {
    CFG* cfg = createTestCFG2();
    SSAGenerator ssaGen;
    ssaGen.computeDominators(cfg);
    const auto& dom = ssaGen.getDom();

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
    
    // Test dominators for each block
    EXPECT_EQ(dom.at(block0).size(), 1);    
    EXPECT_TRUE(dom.at(block0).count(block0) > 0);

    EXPECT_EQ(dom.at(block1).size(), 2);
    EXPECT_TRUE(dom.at(block1).count(block0) > 0);
    EXPECT_TRUE(dom.at(block1).count(block1) > 0);

    EXPECT_EQ(dom.at(block2).size(), 3);
    EXPECT_TRUE(dom.at(block2).count(block0) > 0);
    EXPECT_TRUE(dom.at(block2).count(block1) > 0);
    EXPECT_TRUE(dom.at(block2).count(block2) > 0);

    EXPECT_EQ(dom.at(block3).size(), 3);
    EXPECT_TRUE(dom.at(block3).count(block0) > 0);
    EXPECT_TRUE(dom.at(block3).count(block1) > 0);
    EXPECT_TRUE(dom.at(block3).count(block3) > 0);

    EXPECT_EQ(dom.at(block4).size(), 4);
    EXPECT_TRUE(dom.at(block4).count(block0) > 0);
    EXPECT_TRUE(dom.at(block4).count(block1) > 0);
    EXPECT_TRUE(dom.at(block4).count(block3) > 0);
    EXPECT_TRUE(dom.at(block4).count(block4) > 0);

    EXPECT_EQ(dom.at(block5).size(), 3);
    EXPECT_TRUE(dom.at(block5).count(block0) > 0);
    EXPECT_TRUE(dom.at(block5).count(block1) > 0);
    EXPECT_TRUE(dom.at(block5).count(block5) > 0);

    EXPECT_EQ(dom.at(block6).size(), 4);
    EXPECT_TRUE(dom.at(block6).count(block0) > 0);
    EXPECT_TRUE(dom.at(block6).count(block1) > 0);
    EXPECT_TRUE(dom.at(block6).count(block5) > 0);
    EXPECT_TRUE(dom.at(block6).count(block6) > 0);

    EXPECT_EQ(dom.at(block7).size(), 4);    
    EXPECT_TRUE(dom.at(block7).count(block0) > 0);
    EXPECT_TRUE(dom.at(block7).count(block1) > 0);
    EXPECT_TRUE(dom.at(block7).count(block5) > 0);
    EXPECT_TRUE(dom.at(block7).count(block7) > 0);

    EXPECT_EQ(dom.at(block8).size(), 4);
    EXPECT_TRUE(dom.at(block8).count(block0) > 0);
    EXPECT_TRUE(dom.at(block8).count(block1) > 0);
    EXPECT_TRUE(dom.at(block8).count(block5) > 0);
    EXPECT_TRUE(dom.at(block8).count(block8) > 0);
    

    delete cfg;
    
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}