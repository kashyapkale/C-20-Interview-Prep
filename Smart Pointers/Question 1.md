The goal is to complete the `Project` class to correctly manage `Engineer` objects according to specific ownership rules.

-----

### \#\# The Exercise: Managing a Project Team

You are building a system to manage engineering projects. A project has one **lead engineer** and a **team of member engineers**.

The ownership rules are:

1.  A `Project` has **exclusive ownership** of its `Lead Engineer`. If the project is shut down, the lead engineer is let go immediately. A lead engineer cannot be in charge of more than one project.
2.  Team members, however, are part of a general pool. A `Project` **shares ownership** of its `Team Members`. An engineer can be a member of multiple project teams. They are only let go when the last project they are on is shut down.

Your task is to use the correct smart pointers to enforce these rules.

-----

### \#\# 📝 Your Task

Complete the `Project` class in the code below. You will need to:

1.  Choose the correct smart pointer types for `lead_engineer` and `team_members`.
2.  Implement the `set_lead_engineer` and `add_team_member` methods.
3.  Ensure that memory is managed correctly according to the rules.

<!-- end list -->

```cpp
#include <iostream>
#include <string>
#include <vector>
#include <memory> // Don't forget this header!

// GIVEN CODE: You do not need to change the Engineer class.
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

// YOUR TASK: Complete this class.
class Project {
private:
    std::string project_name_;
    // TODO: Choose the correct smart pointer for the lead engineer.
    // HINT: Exclusive ownership.
    /* YOUR CODE HERE */

    // TODO: Choose the correct smart pointer collection for team members.
    // HINT: Shared ownership.
    /* YOUR CODE HERE */

public:
    Project(const std::string& name) : project_name_(name) {
        std::cout << "\n[PROJ] Starting project '" << project_name_ << "'." << std::endl;
    }
    ~Project() {
        std::cout << "[PROJ] Project '" << project_name_ << "' is finished." << std::endl;
    }

    // TODO: Implement this method. It should take ownership of the engineer.
    void set_lead_engineer(/* YOUR CODE HERE */) {
        /* YOUR CODE HERE */
        std::cout << "      " << lead_engineer->get_name() << " is now the lead engineer." << std::endl;
    }

    // TODO: Implement this method. It should share ownership of the engineer.
    void add_team_member(/* YOUR CODE HERE */) {
        /* YOUR CODE HERE */
        std::cout << "      " << member->get_name() << " has joined the team." << std::endl;
    }

    void run_project() const {
        std::cout << "      Running project '" << project_name_ << "'..." << std::endl;
        if (lead_engineer) {
            lead_engineer->work();
        }
        for (const auto& member : team_members) {
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
```

-----

### \#\# ✅ Expected Output

When your implementation is correct, running the `main` function should produce this exact output:

```text
--- TEST CASE 1: Basic Project Lifecycle ---
  [+] Engineer Bob is on the job.

[PROJ] Starting project 'Alpha'.
  [+] Engineer Alice is on the job.
      Alice is now the lead engineer.
      Bob has joined the team.
      Running project 'Alpha'...
      Engineer Alice is working hard.
      Engineer Bob is working hard.
Bob's use count outside project: 2
[PROJ] Project 'Alpha' is finished.
  [-] Engineer Bob is leaving.
  [-] Engineer Alice is leaving.

--- TEST CASE 2: Transferring Lead Engineer ---
  [+] Engineer Charlie is on the job.

[PROJ] Starting project 'Beta'.
      Charlie is now the lead engineer.
      Ownership of Charlie successfully transferred to Project Beta.
      Running project 'Beta'...
      Engineer Charlie is working hard.
[PROJ] Project 'Beta' is finished.
  [-] Engineer Charlie is leaving.

--- TEST CASE 3: Shared Team Member ---
  [+] Engineer Diana is on the job.
Diana's initial use count: 1

[PROJ] Starting project 'Gamma'.
      Diana has joined the team.
Diana's use count after joining Gamma: 2

[PROJ] Starting project 'Delta'.
      Diana has joined the team.
Diana's use count after joining Delta: 3
[PROJ] Project 'Delta' is finished.
Diana's use count after Delta finished: 2
[PROJ] Project 'Gamma' is finished.
  [-] Engineer Diana is leaving.
```
