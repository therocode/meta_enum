#include <array>
#include <string_view>
#include <iostream>

template <typename EnumType>
struct MetaEnumMember
{
    EnumType value = {};
    std::string_view name;
    std::string_view string;
    size_t index = {};
};

template <typename EnumType, size_t size>
struct EnumMeta
{
    std::string_view string;
    std::array<MetaEnumMember<EnumType>, size> members;
};

constexpr size_t nextEnumCommaOrEnd(size_t start, std::string_view enumString)
{
    size_t brackets = 0; //()[]{}
    bool quote = false; //""
    char lastChar = '\0';

    auto isNested = [&brackets, &quote] ()
    {
        return brackets != 0 || quote;
    };

    auto feedCounters = [&brackets, &quote, lastChar] (char c)
    {
        switch(c)
        {
            case '"':
                if(lastChar != '\\') //ignore " if they are backslashed
                    quote = !quote;
                break;
            case '(':
            case '<':
            case '{':
                ++brackets;
                break;
            case ')':
            case '>':
            case '}':
                --brackets;
                break;
            default:
                break;
        }
    };

    size_t current = start;
    for(; current < enumString.size() && (isNested() || enumString[current] != ','); ++current)
    {
        feedCounters(enumString[current]);
        lastChar = enumString[current];
    }

    return current;
}

constexpr size_t countEnumMembers(std::string_view in)
{
    size_t result = 0;

    size_t currentStringIndex = 0;
    char current = ',';

    while(current == ',')
    {
        ++result;
        currentStringIndex = nextEnumCommaOrEnd(currentStringIndex + 1, in);

        if(currentStringIndex == in.size())
            break;

        current = in[currentStringIndex];
    }

    return result;
}

constexpr std::string_view parseEnumMemberName(std::string_view memberString)
{
    std::string_view memberName = memberString;

    //TODO: parse name

    return memberName;
}

template <typename EnumType, size_t size>
constexpr EnumMeta<EnumType, size> parseEnumMeta(std::string_view in, const std::array<EnumType, size>& values)
{
    EnumMeta<EnumType, size> result;
    result.string = in;

    std::array<std::string_view, size> memberStrings;
    size_t amountFilled = 0;

    size_t currentStringStart = 0;

    while(amountFilled < size)
    {
        size_t currentStringEnd = nextEnumCommaOrEnd(currentStringStart + 1, in);
        size_t currentStringSize = currentStringEnd - currentStringStart;
        
        if(currentStringStart != 0)
        {
            ++currentStringStart;
            --currentStringSize;
        }

        memberStrings[amountFilled] = std::string_view(in.begin() + currentStringStart, currentStringSize);
        ++amountFilled;
        currentStringStart = currentStringEnd;
    }

    for(size_t i = 0; i < memberStrings.size(); ++i)
    {
        result.members[i].name = parseEnumMemberName(memberStrings[i]);
        result.members[i].string = memberStrings[i];
        result.members[i].value = values[i];
        result.members[i].index = i;
    }

    return result;
}

template <typename EnumUnderlyingType>
struct IntWrapper
{
    constexpr IntWrapper(): value(0), empty(true)
    {
    }
    constexpr IntWrapper(EnumUnderlyingType in): value(in), empty(false)
    {
    }
    constexpr IntWrapper operator=(EnumUnderlyingType in)
    {
        value = in;
        empty = false;
        return *this;
    }
    EnumUnderlyingType value;
    bool empty;
};

template <typename EnumType, typename EnumUnderlyingType, size_t size>
constexpr std::array<EnumType, size> resolveEnumValuesArray(const std::initializer_list<IntWrapper<EnumUnderlyingType>>& in)
{
    std::array<EnumType, size> result{};

    EnumUnderlyingType nextValue = 0;
    for(size_t i = 0; i < size; ++i)
    {
        auto wrapper = *(in.begin() + i);
        EnumUnderlyingType newValue = wrapper.empty ? nextValue : wrapper.value;
        nextValue = newValue + 1;
        result[i] = static_cast<EnumType>(newValue);
    }

    return result;
}

#define meta_enum(Type, ...)\
    enum Type { __VA_ARGS__};\
    using Type##_underlying_type = int;\
    constexpr static size_t Type##_internal_size = countEnumMembers(#__VA_ARGS__);\
    struct Type##_internal_emulator\
    {\
        constexpr std::array<Type, Type##_internal_size> emulateEnumValues()\
        {\
            using IntWrapperType = IntWrapper<Type##_underlying_type>;\
            IntWrapperType __VA_ARGS__;\
            return resolveEnumValuesArray<Type, Type##_underlying_type, Type##_internal_size>({__VA_ARGS__});\
        }\
    };\
    constexpr static auto Type##_meta = parseEnumMeta<Type, Type##_internal_size>(#__VA_ARGS__, Type##_internal_emulator{}.emulateEnumValues());

////USAGE:

meta_enum(Hahas, Hi, Ho= 2, Hu =     4, 
He);

//declares enum as Hahas like usual.
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

////TODO

/* Errors with enum nested in struct... something about constexpr not being used from non-complete classes
struct T
{
    meta_enum(TEnum, One, Two
    =
    0b11010101, Three
    );
};
*/

//enum class support

//parse enum member name

//errors on clang :'(
