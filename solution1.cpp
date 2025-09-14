#include <iostream>
#include <string>
#include <vector>
#include <memory> 
using namespace std;

class Engineer {
private:
    std::string name_;
public:
    Engineer(const std::string& name) : name_(name) {
        std::cout << "  [+] Engineer " << name_ << " is on the job." << std::endl;
    }
    ~Engineer() {
        std::cout << "  [-] Engineer " << name_ << " is leaving." << std::endl;
    }
    void work() const {
        std::cout << "      Engineer " << name_ << " is working hard." << std::endl;
    }
    const std::string& get_name() const { return name_; }
};

class Project {
private:
    std::string project_name_;
    unique_ptr<Engineer> lead_engineer;
    vector<shared_ptr<Engineer>> engineer_pool;

public:
    Project(const std::string& name) : project_name_(name) {
        std::cout << "\n[PROJ] Starting project '" << project_name_ << "'." << std::endl;
    }
    ~Project() {
        std::cout << "[PROJ] Project '" << project_name_ << "' is finished." << std::endl;
    }

    // TODO: Implement this method. It should take ownership of the engineer.
    void set_lead_engineer(unique_ptr<Engineer> lead_engineer_) {
        /* YOUR CODE HERE */
        this->lead_engineer = std::move(lead_engineer_);
        std::cout << "      " << lead_engineer->get_name() << " is now the lead engineer." << std::endl;
    }

    // TODO: Implement this method. It should share ownership of the engineer.
    void add_team_member(shared_ptr<Engineer> member) {
        engineer_pool.push_back(member);
        std::cout << "      " << member->get_name() << " has joined the team." << std::endl;
    }

    void run_project() const {
        std::cout << "      Running project '" << project_name_ << "'..." << std::endl;
        if (lead_engineer) {
            lead_engineer->work();
        }
        for (const auto& member : engineer_pool) {
            member->work();
        }
    }
};

// TEST CASES: Do not change the main function.
int main() {
    std::cout << "--- TEST CASE 1: Basic Project Lifecycle ---" << std::endl;
    {
        // Create a standalone engineer who can be shared
        auto shared_engineer_bob = std::make_shared<Engineer>("Bob");

        Project alpha("Alpha");
        
        // Lead engineer is created and ownership is immediately moved to the project
        alpha.set_lead_engineer(std::make_unique<Engineer>("Alice"));
        alpha.add_team_member(shared_engineer_bob);

        alpha.run_project();
        
        std::cout << "Bob's use count outside project: " << shared_engineer_bob.use_count() << std::endl; // Should be 2
    } // Project Alpha goes out of scope here. Alice and Bob should be "leaving".
    
    std::cout << "\n--- TEST CASE 2: Transferring Lead Engineer ---" << std::endl;
    {
        auto lead_charlie = std::make_unique<Engineer>("Charlie");
        
        Project beta("Beta");
        // Move ownership of Charlie to Project Beta
        beta.set_lead_engineer(std::move(lead_charlie));
        
        // After the move, lead_charlie in this scope should be null
        if (!lead_charlie) {
            std::cout << "      Ownership of Charlie successfully transferred to Project Beta." << std::endl;
        }

        beta.run_project();
    } // Project Beta goes out of scope. Charlie should be "leaving".

    std::cout << "\n--- TEST CASE 3: Shared Team Member ---" << std::endl;
    {
        auto shared_engineer_diana = std::make_shared<Engineer>("Diana");
        std::cout << "Diana's initial use count: " << shared_engineer_diana.use_count() << std::endl; // Should be 1

        Project gamma("Gamma");
        gamma.add_team_member(shared_engineer_diana);
        std::cout << "Diana's use count after joining Gamma: " << shared_engineer_diana.use_count() << std::endl; // Should be 2

        {
            Project delta("Delta");
            delta.add_team_member(shared_engineer_diana);
            std::cout << "Diana's use count after joining Delta: " << shared_engineer_diana.use_count() << std::endl; // Should be 3

        } // Project Delta is finished. Diana should NOT be "leaving" yet.

        std::cout << "Diana's use count after Delta finished: " << shared_engineer_diana.use_count() << std::endl; // Should be 2
    } // Project Gamma is finished. The original shared_ptr also goes out of scope. Now Diana should be "leaving".


    return 0;
}