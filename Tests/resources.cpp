#include <catch.hpp>

#include <Util/ResourceManager.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio/Music.hpp>

TEST_CASE("Resource Manager, with a single texture resource", "[resource][refcount]")
{
    Util::ResourceManager<sf::Texture> textures;

    textures.add("TestTexture", "TestTexture.png");

    CHECK(textures.has("TestTexture"));

    auto& res = *textures.get("TestTexture");

    auto tex = *res;
    
    CHECK(tex.getSize() == sf::Vector2u(64, 53));

    res.GCRelease();

    CHECK(textures.has("TestTexture"));
    CHECK(((sf::Texture*)res == nullptr));

    auto& res2 = *textures.get("TestTexture");

    tex = *res2;
    
    CHECK(tex.getSize() == sf::Vector2u(64, 53));
}

TEST_CASE("Resource Manager, with a single music resource", "[resource][refcount]")
{
    Util::ResourceManager<sf::Music> musics;

    musics.add("TestSong", "TestSong.ogg");

    CHECK(musics.has("TestSong"));

    auto& res = *musics.get("TestSong");

    auto& mus = *res;

    CHECK(mus.getDuration() > sf::seconds(0));
}