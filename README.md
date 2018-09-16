### Meta Enum

A way to declare enums so that meta-information about the enum such as names and indices of the enum values is available as constexpr facilities.

check main.cpp for code/usage or just go here: https://godbolt.org/z/1xib2U

## Usage

```cpp
//first argument is enum name, then the values follow
meta_enum(Hahas, Hi, Ho= 2, Hu =     4,
He);

//declares enum as Hahas like a usual usual enum.
//Provides meta object accessible with Hahas_meta

int main()
{
    static_assert(Hahas_meta.members[0].string == "Hi");
    static_assert(Hahas_meta.members[1].string == " Ho= 2");
    static_assert(Hahas_meta.members[2].string == " Hu = 4");
    static_assert(Hahas_meta.members[3].string == " He");

    //TODO:
    //static_assert(Hahas_meta.members[0].name == "Hi");
    //static_assert(Hahas_meta.members[1].name == "Ho");
    //static_assert(Hahas_meta.members[2].name == "Hu");
    //static_assert(Hahas_meta.members[3].name == "He");

    static_assert(Hahas_meta.members[0].value == 0);
    static_assert(Hahas_meta.members[1].value == 2);
    static_assert(Hahas_meta.members[2].value == 4);
    static_assert(Hahas_meta.members[3].value == 5);

    static_assert(Hahas_meta.members[0].index == 0);
    static_assert(Hahas_meta.members[1].index == 1);
    static_assert(Hahas_meta.members[2].index == 2);
    static_assert(Hahas_meta.members[3].index == 3);

    std::cout << "declared enum: " << Hahas_meta.string << "\n";
    std::cout << "member strings: \n";
    for(const auto& enumMember : Hahas_meta.members)
    {
        std::cout << enumMember.string << "\n";
    }
}
```
