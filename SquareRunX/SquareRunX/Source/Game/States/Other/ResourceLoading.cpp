#include "ResourceLoading.h"
#include "Game/States/Menus/MainMenu.h"

#include <thread>
#include <chrono>
#include <mutex>

using namespace std::chrono_literals;

namespace Threading
{
    std::mutex Mutex; // Ensure safe access of variables shared by multiple threads
}

void ResourceLoading::LoadGameFonts()
{
    // Load the fonts required by the game
    this->GameFonts[0] = { "Arial-Rounded",
        FontLoaderTTF::GetSingleton().LoadFont("ARIAL_ROUNDED", "GameFiles/Fonts/ARLRDBD.TTF") };
    this->GameFonts[1] = { "Sans-Serif-Shaded",
        FontLoaderTTF::GetSingleton().LoadFont("SANS_SERIF_SHADED", "GameFiles/Fonts/SANSSERIFSH.TTF") };
    this->GameFonts[2] = { "Discovery",
        FontLoaderTTF::GetSingleton().LoadFont("DISCOVERY", "GameFiles/Fonts/DISCOVERY.TTF") };
}

void ResourceLoading::LoadGameTextures()
{
    // Load the textures required by the game
    this->GameTextures[0] = { "Loading-Wheel",
        TextureManager::GetSingleton().LoadTexture("LOADING_WHEEL", "GameFiles/Textures/LOADING_WHEEL.png") };
    this->GameTextures[1] = { "Tile-Sheet",
        TextureManager::GetSingleton().LoadTexture("TILE_SHEET", "GameFiles/Textures/TILE_SHEET.png") };
    this->GameTextures[2] = { "Button-Img",
        TextureManager::GetSingleton().LoadTexture("BUTTON_BKG", "GameFiles/Textures/BUTTON_BKG.png") };
    this->GameTextures[3] = { "Keybinds-Img",
        TextureManager::GetSingleton().LoadTexture("KEYBINDS_IMG", "GameFiles/Textures/KEYBIND_DISPLAY.png") };
    this->GameTextures[4] = { "Transition-1",
        TextureManager::GetSingleton().LoadTexture("TRANSITION_1", "GameFiles/Textures/TRANSITION_1.png") };
    this->GameTextures[5] = { "Transition-2",
        TextureManager::GetSingleton().LoadTexture("TRANSITION_2", "GameFiles/Textures/TRANSITION_2.png") };
    this->GameTextures[6] = { "Leaderboard-Skin",
        TextureManager::GetSingleton().LoadTexture("LEADERBOARD_BKG", "GameFiles/Textures/LEADERBOARD_BKG.png") };
    this->GameTextures[7] = { "Button-Box",
        TextureManager::GetSingleton().LoadTexture("BUTTON_BOX", "GameFiles/Textures/BUTTON_BOX.png") };
    this->GameTextures[8] = { "Text-Box",
        TextureManager::GetSingleton().LoadTexture("TEXT_BOX", "GameFiles/Textures/TEXT_BOX.png") };
    this->GameTextures[9] = { "Transition-3",
        TextureManager::GetSingleton().LoadTexture("TRANSITION_3", "GameFiles/Textures/TRANSITION_3.png") };
    this->GameTextures[10] = { "Player-Spritesheet",
        TextureManager::GetSingleton().LoadTexture("PLAYER_SPRITESHEET", "GameFiles/Textures/PLAYER_SPRITESHEET.png") };

    // Load parallax background textures
    this->GameTextures[11] = { "Forest-Clouds",
       TextureManager::GetSingleton().LoadTexture("FOREST_PX_3", "GameFiles/Textures/Forest/3.png") };
    this->GameTextures[12] = { "Forest-Close",
       TextureManager::GetSingleton().LoadTexture("FOREST_PX_6", "GameFiles/Textures/Forest/6.png") };
    this->GameTextures[13] = { "Forest-Middle",
       TextureManager::GetSingleton().LoadTexture("FOREST_PX_5", "GameFiles/Textures/Forest/5.png") };
    this->GameTextures[14] = { "Forest-Far",
       TextureManager::GetSingleton().LoadTexture("FOREST_PX_4", "GameFiles/Textures/Forest/4.png") };
    this->GameTextures[15] = { "Forest-Mountains",
       TextureManager::GetSingleton().LoadTexture("FOREST_PX_2", "GameFiles/Textures/Forest/2.png") };
    this->GameTextures[16] = { "Forest-Sky",
       TextureManager::GetSingleton().LoadTexture("FOREST_PX_1", "GameFiles/Textures/Forest/1.png") };
}

void ResourceLoading::LoadGameAudio()
{
    // Load the music and sound effects required by the game
}

void ResourceLoading::LoadGameLevelMaps()
{
    std::this_thread::sleep_for(2s); // Pause the loading thread for a bit

    // Load the playable levels required by the game
    FileHandler::GetSingleton().OpenFile("LEVEL_DATABASE", "GameFiles/Data/LevelDatabase.dat");

    for (int i = 0; i < FileHandler::GetSingleton().GetNumLinesInFile("LEVEL_DATABASE"); i++)
    { 
        const std::string LOADED_LEVEL_PATH = FileHandler::GetSingleton().ReadData("LEVEL_DATABASE", i + 1).str();
        LevelMap LoadedLevel = LevelMap(LOADED_LEVEL_PATH);

        this->GameLevels.emplace_back(LoadedLevel);
    }

    FileHandler::GetSingleton().CloseFile("LEVEL_DATABASE");

    // Lock access to variable from other threads temporarily
    Threading::Mutex.lock();
    this->FinishedLoading = true;
    Threading::Mutex.unlock();
}

void ResourceLoading::InitState()
{
    this->LoadGameFonts();
    this->LoadGameTextures();
    this->LoadGameAudio();

    // Start loading the playable levels of the game
    std::thread LevelLoadingThread(&ResourceLoading::LoadGameLevelMaps, this);
    LevelLoadingThread.detach();
}

void ResourceLoading::DestroyState() {}

void ResourceLoading::UpdateTick(const double& DeltaTime)
{
    if(this->FinishedLoading)
        this->SwitchState(MainMenu::GetGameState());

    this->LoadingWheelRotation += 0.425 * DeltaTime; // Update rotation position of the loading wheel
}

void ResourceLoading::RenderFrame() const
{
    // Clear the screen and fill with dark grey color
    GraphicsRenderer::GetSingleton().SetClearColor(glm::vec3(0.1f));

    // Render text and loading wheel to screen
    Rect SourceRect, DestinationRect;
    SourceRect = { 0, 0, 800, 800 };
    DestinationRect = { (int)(this->SceneCamera.GetViewSize().x / 2), (int)(this->SceneCamera.GetViewSize().y / 2) - 400, 
        60, 60 };

    GraphicsRenderer::GetSingleton().RenderQuad(SourceRect, DestinationRect, *this->GetTexture("Loading-Wheel"), 
        this->LoadingWheelRotation);
}

ResourceLoading* ResourceLoading::GetGameState()
{
    static ResourceLoading State;
    return &State;
}

const Font* ResourceLoading::GetFont(const std::string& FontName) const
{
    for (const auto& Obj : this->GameFonts)
    {
        if (Obj.FontName == FontName)
            return Obj.Data;
    }

    return nullptr;
}

const Texture* ResourceLoading::GetTexture(const std::string& TextureName) const
{
    for (const auto& Obj : this->GameTextures)
    {
        if (Obj.TextureName == TextureName)
            return Obj.Data;
    }

    return nullptr;
}

LevelMap* ResourceLoading::GetLevelMap(int LevelIndex, int ActIndex) const
{
    for (auto& Obj : this->GameLevels)
    {
        if (Obj.GetLevelIndex() == LevelIndex && Obj.GetActIndex() == ActIndex)
            return &Obj;
    }

    return nullptr;
}