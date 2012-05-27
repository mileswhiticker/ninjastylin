#include "App.hpp"

#include "PlayerEntity.hpp"
#include "Shuriken.hpp"

App::App(void)
:	mRoot(0)
,	mCamera(0)
,	mSceneMgr(0)
,	mCameraMan(0)
,	m_pPlayerEntity(0)
,	m_pMainlight(0)
	//
,	mWindow(0)
,	mResourcesCfg(Ogre::StringUtil::BLANK)
,	mPluginsCfg(Ogre::StringUtil::BLANK)
,	mTrayMgr(0)
,	mDetailsPanel(0)
	//
,	mShutDown(false)
,	m_SceneSetup(false)
	//
,	mCursorWasVisible(false)
,	mInputManager(0)
,	mMouse(0)
,	mKeyboard(0)
	//
,	m_pSheet(NULL)
,	m_pTitle(NULL)
,	m_pQuit(NULL)
,	m_pNewGameButton(NULL)
,	m_pMainMenuButton(NULL)
	//
,	tLeftShurkenAlive(SHURIKEN_TTL)
,	m_pThrownShuriken(NULL)
{
	//
}

//-------------------------------------------------------------------------------------
App::~App(void)
{
    if (mTrayMgr) delete mTrayMgr;
    if (mCameraMan) delete mCameraMan;

    //Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);
    delete mRoot;
}

//-------------------------------------------------------------------------------------
bool App::configure(void)
{
    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    if(mRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        mWindow = mRoot->initialise(true, "Ninja Stylin' Prototype");

        return true;
    }
    else
    {
        return false;
    }
}
//-------------------------------------------------------------------------------------
void App::chooseSceneManager(void)
{
    // Get the SceneManager
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_EXTERIOR_CLOSE);
}
//-------------------------------------------------------------------------------------
void App::createCamera(void)
{
    // Create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");

	//view distance limits
    mCamera->setNearClipDistance(0.1f);
	mCamera->setFarClipDistance(50000);
 
	// Set camera look point
	mCamera->setPosition(40, 1000, 580);
	mCamera->pitch(Ogre::Degree(-30));
	mCamera->yaw(Ogre::Degree(-45));

	if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_INFINITE_FAR_PLANE))
	{
		mCamera->setFarClipDistance(0);   // enable infinite far clip distance if we can
	}
    mCameraMan = new OgreBites::SdkCameraMan(mCamera);  
}
//-------------------------------------------------------------------------------------
void App::createFrameListener(void)
{
    Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;
 
    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
 
    mInputManager = OIS::InputManager::createInputSystem( pl );
 
    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
    mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));
 
    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);
 
    //Set initial mouse clipping size
    windowResized(mWindow);
 
    //Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
 
    mRoot->addFrameListener(this);

	mCount = 0;
	mCurrentObject = NULL;
	mLMouseDown = false;
	mRMouseDown = false;
 
	// Reduce rotate speed
	mRotateSpeed = 0.1f;

	//create ray query obj
	mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());
}
//-------------------------------------------------------------------------------------
void App::createViewports(void)
{
    // Create one viewport, entire window
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(
        Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}
//-------------------------------------------------------------------------------------
void App::setupResources(void)
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(mResourcesCfg);

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
}
//-------------------------------------------------------------------------------------
void App::createResourceListener(void)
{

}
//-------------------------------------------------------------------------------------
void App::loadResources(void)
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//-------------------------------------------------------------------------------------
void App::go(void)
{
#ifdef _DEBUG
    mResourcesCfg = "resources_d.cfg";
    mPluginsCfg = "plugins_d.cfg";
#else
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
#endif

    if (!setup())
        return;
	
	Ogre::Timer timer;
	//mRoot->startRendering();
	float lastTime = timer.getMicroseconds()/1000000.0f;
	float curTime = timer.getMicroseconds()/1000000.0f;
	while(update(curTime - lastTime))
	{
		lastTime = curTime;
		curTime = timer.getMicroseconds()/1000000.0f;
		//timer.reset();
	}

    // clean up
    destroyScene();
}
//-------------------------------------------------------------------------------------
bool App::update(float a_DeltaT)
{
	//get window messages
	MSG  msg;
	while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//update player / shuriken
	if(m_pThrownShuriken)
	{
		m_pThrownShuriken->Update(a_DeltaT);
		//
		if(!m_pThrownShuriken->GetMoveForward() && tLeftShurkenAlive > 3.f)
			tLeftShurkenAlive = 3.f;
		tLeftShurkenAlive -= a_DeltaT;
		if(tLeftShurkenAlive < 0)
		{
			delete m_pThrownShuriken;
			m_pThrownShuriken = NULL;
			tLeftShurkenAlive = SHURIKEN_TTL;
		}
	}
	else if(m_pPlayerEntity)
		m_pPlayerEntity->Update(a_DeltaT);
	
	//update all other entities
	for(unsigned short n=0;n<m_Mobs.size();n++)
	{
		if(m_Mobs[n])
			m_Mobs[n]->Update(a_DeltaT);
	}

	//handle input injection and misc other
	Ogre::FrameEvent frameEvent;
	frameRenderingQueued(frameEvent);

	//render a single frame
	return mRoot->renderOneFrame();
}
//-------------------------------------------------------------------------------------
bool App::setup(void)
{
    mRoot = new Ogre::Root(mPluginsCfg);
    setupResources();
    bool carryOn = configure();
    if (!carryOn) return false;

    chooseSceneManager();
	createCamera();
    createViewports();

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    // Create any resource listeners (for loading screens)
    createResourceListener();
    // Load resources
    loadResources();
	
	//initialise cegui
	mRenderer = &CEGUI::OgreRenderer::bootstrapSystem();

	//setup cegui resource paths
	CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

	//some theme things
	CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
	CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");

	//setup the gui
	setupGUI();

    createFrameListener();

    return true;
};
//-------------------------------------------------------------------------------------
bool App::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if(mWindow->isClosed())
        return false;
 
    if(mShutDown)
        return false;
 
    //Need to capture/update each device
    mKeyboard->capture();
    mMouse->capture();
 
    //Need to inject timestamps to CEGUI System.
    CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);

	//---- handle terrain collisions

	//raycast to the ground from the player
	Ogre::Vector3 startPos = Ogre::Vector3(0,1000.f,0);
	if(m_pThrownShuriken)
		startPos = m_pThrownShuriken->GetPosition();
	else if(m_pPlayerEntity)
		startPos = m_pPlayerEntity->GetPosition();
	//
	Ogre::Ray ray(Ogre::Vector3(startPos.x, 5000.0f, startPos.z), Ogre::Vector3::NEGATIVE_UNIT_Y);
	mRaySceneQuery->setRay(ray);
	Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
	Ogre::RaySceneQueryResult::iterator itr = result.begin();

	//tell the player about the ground height
	if (itr != result.end() && itr->worldFragment)
	{
		if(m_pThrownShuriken)
			m_pThrownShuriken->SetGroundHeight(itr->worldFragment->singleIntersection.y);
		else
			m_pPlayerEntity->SetGroundHeight(itr->worldFragment->singleIntersection.y);
	}

	//
	return true;
}
//-------------------------------------------------------------------------------------
bool App::keyPressed( const OIS::KeyEvent &arg )
{
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.injectKeyDown(arg.key);
	sys.injectChar(arg.text);

	switch(arg.key)
	{
	case(OIS::KC_W):
		{
			if(!m_pThrownShuriken)
				m_pPlayerEntity->SetMoveForward(1);
			break;
		}
	case(OIS::KC_S):
		{
			if(!m_pThrownShuriken)
				m_pPlayerEntity->SetMoveForward(-1);
			break;
		}
	case(OIS::KC_A):
		{
			if(!m_pThrownShuriken)
				m_pPlayerEntity->SetMoveSide(1);
			else
				m_pThrownShuriken->SetMoveSide(1);
			break;
		}
	case(OIS::KC_D):
		{
			if(!m_pThrownShuriken)
				m_pPlayerEntity->SetMoveSide(-1);
			else
				m_pThrownShuriken->SetMoveSide(-1);
			break;
		}
		//
	case(OIS::KC_SPACE):
		{
			//create a shuriken
			if(!m_pThrownShuriken)
				ThrowShuriken();
			break;
		}
	case(OIS::KC_RSHIFT):
		{
			//fall through
		}
	case(OIS::KC_LSHIFT):
		{
			//move at double speed
			if(!m_pThrownShuriken)
				m_pPlayerEntity->SetMoveForwardMultiplier(2);
			else
				m_pThrownShuriken->SetMoveForwardMultiplier(2);
			break;
		}
		//
	case(OIS::KC_ESCAPE):
		{
			mShutDown = true;
			break;
		}
	}
    mCameraMan->injectKeyDown(arg);
    return true;
}

bool App::keyReleased( const OIS::KeyEvent &arg )
{
	CEGUI::System::getSingleton().injectKeyUp(arg.key);
	
	switch(arg.key)
	{
	case(OIS::KC_W):
		{
			if(!m_pThrownShuriken)
				m_pPlayerEntity->SetMoveForward(0);
			else
				m_pThrownShuriken->SetMoveForward(0);
			break;
		}
	case(OIS::KC_S):
		{
			if(!m_pThrownShuriken)
				m_pPlayerEntity->SetMoveForward(0);
			else
				m_pThrownShuriken->SetMoveForward(0);
			break;
		}
	case(OIS::KC_A):
		{
			if(!m_pThrownShuriken)
				m_pPlayerEntity->SetMoveSide(0);
			else
				m_pThrownShuriken->SetMoveSide(0);
			break;
		}
	case(OIS::KC_D):
		{
			if(!m_pThrownShuriken)
				m_pPlayerEntity->SetMoveSide(0);
			else
				m_pThrownShuriken->SetMoveSide(0);
			break;
		}
	case(OIS::KC_RSHIFT):
		{
			//fall through
		}
	case(OIS::KC_LSHIFT):
		{
			//stop moving at double speed
			m_pPlayerEntity->SetMoveForwardMultiplier(1);
			break;
		}
	}
    //mCameraMan->injectKeyUp(arg);
    return true;
}

CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID)
{
    switch (buttonID)
    {
    case OIS::MB_Left:
        return CEGUI::LeftButton;
 
    case OIS::MB_Right:
        return CEGUI::RightButton;
 
    case OIS::MB_Middle:
        return CEGUI::MiddleButton;
 
    default:
        return CEGUI::LeftButton;
    }
}

bool App::mouseMoved( const OIS::MouseEvent &arg )
{
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
	// Scroll wheel.
	if (arg.state.Z.rel)
		sys.injectMouseWheelChange(arg.state.Z.rel / 120.0f);
	
	//mouse move horizontal
	/*if(arg.state.X.rel && m_pPlayerEntity)
		m_pPlayerEntity->SetLookHoriz(arg.state.X.rel / 120.0f);

	//mouse move vert
	if(arg.state.Y.rel && m_pPlayerEntity)
		m_pPlayerEntity->SetLookVert(arg.state.Y.rel / 120.0f);*/
	if (mLMouseDown)
	{
		//nothing
	}
	else if (mRMouseDown)
	{
		//rotate camera
		mCamera->yaw(Ogre::Degree(-arg.state.X.rel * mRotateSpeed));
		mCamera->pitch(Ogre::Degree(-arg.state.Y.rel * mRotateSpeed));
		//m_pPlayerEntity->Yaw(Ogre::Radian(-arg.state.X.rel * mRotateSpeed));
	}
    /*if (mTrayMgr->injectMouseMove(arg)) return true;
    mCameraMan->injectMouseMove(arg);*/
    return true;
}

bool App::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	CEGUI::System::getSingleton().injectMouseButtonDown(convertButton(id));
	//
	if (id == OIS::MB_Left)
	{
		mLMouseDown = true;
	}
	else if (id == OIS::MB_Right)
	{
		CEGUI::MouseCursor::getSingleton().hide();
		mRMouseDown = true;
	}
	//
    return true;
}

bool App::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	CEGUI::System::getSingleton().injectMouseButtonUp(convertButton(id));
	//
	if (id == OIS::MB_Left)
	{
		mLMouseDown = false;
	}
	else if (id == OIS::MB_Right)
	{
		CEGUI::MouseCursor::getSingleton().show();
		mRMouseDown = false;
	}
	//
    return true;
}

//Adjust mouse clipping area
void App::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void App::windowClosed(Ogre::RenderWindow* rw)
{
    //Only close for window that created OIS (the main window in these demos)
    if( rw == mWindow )
    {
        if( mInputManager )
        {
            mInputManager->destroyInputObject( mMouse );
            mInputManager->destroyInputObject( mKeyboard );

            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
        }
    }
}

void App::createScene(void)
{
	//change the gui
	m_pQuit->setPosition( CEGUI::UVector2( CEGUI::UDim(0.75f,0), CEGUI::UDim(0.05f,0) ) );
	//
	m_pNewGameButton->setVisible(false);
	m_pMainMenuButton->setVisible(true);
	m_pTitle->setVisible(false);

	// Set the scene's lighting
	Ogre::Vector3 lightdir(0.55f, -0.3f, 0.75f);
	lightdir.normalise();
 
	m_pMainlight = mSceneMgr->createLight("MainLight");
	m_pMainlight->setType(Ogre::Light::LT_DIRECTIONAL);
	m_pMainlight->setDirection(lightdir);
	m_pMainlight->setDiffuseColour(Ogre::ColourValue::White);
	m_pMainlight->setSpecularColour(Ogre::ColourValue(0.4f, 0.4f, 0.4f));
 
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2f, 0.2f, 0.2f));
	
	// World geometry
	mSceneMgr->setWorldGeometry("terrain.cfg");
 
	//create a player entity
	m_pPlayerEntity = new PlayerEntity("Player", *mSceneMgr, mCamera);

    Ogre::ColourValue fadeColour(0.9f, 0.9f, 0.9f);
    mSceneMgr->setFog(Ogre::FOG_LINEAR, fadeColour, 0.0f, 10, 1200);
    mWindow->getViewport(0)->setBackgroundColour(fadeColour);
 
    Ogre::Plane plane;
    plane.d = 100;
    plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Y;
 
    mSceneMgr->setSkyPlane(true, plane, "Examples/CloudySky", 500, 20, true, 0.5, 150, 150);

	m_SceneSetup = true;
}
//-------------------------------------------------------------------------------------
void App::destroyScene(void)
{
    // clear entities
	/*while(m_Mobs.size())
	{
		delete *m_Mobs.begin();
		m_Mobs.erase(m_Mobs.begin());
	}

	//clear the player
	delete m_pPlayerEntity;
	m_pPlayerEntity = NULL;*/

	//clear lighting
	if(m_pMainlight)
	{
		mSceneMgr->destroyLight(m_pMainlight);
		m_pMainlight = NULL;
	}
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0, 0, 0));

	//clear geometry
	//mSceneMgr->destroyStaticGeometry("terrain.cfg");

	//clear scene
	//mSceneMgr->clearScene();

	//clear ray query
	mSceneMgr->destroyQuery(mRaySceneQuery);

	m_SceneSetup = false;
}
//-------------------------------------------------------------------------------------
void App::setupGUI()
{
	//setup cegui window
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	m_pSheet = wmgr.createWindow("DefaultWindow", "CEGUI/Sheet");
	CEGUI::System::getSingleton().setGUISheet(m_pSheet);
	CEGUI::UVector2 windowSize = m_pSheet->getSize();
	
	//wmgr.loadWindowLayout( "Main.layout" );

	//make an application title
	m_pTitle = wmgr.createWindow("TaharezLook/StaticText", "CEGUI/MainMenuTitle");
	m_pTitle->setText("Ninja Stylin' Prototype rc1");
	m_pTitle->setSize(CEGUI::UVector2(CEGUI::UDim(0.5f, 0), CEGUI::UDim(0.2f, 0)));
	m_pTitle->setPosition( CEGUI::UVector2( CEGUI::UDim(0.25f,0), CEGUI::UDim(0.1f,0) ) );
	m_pSheet->addChildWindow(m_pTitle);
	
	//make a new game button
	m_pNewGameButton = wmgr.createWindow("TaharezLook/Button", "CEGUI/NewGameButton");
	m_pNewGameButton->setText("Run prototype");
	m_pNewGameButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.2f, 0), CEGUI::UDim(0.1f, 0)));
	m_pNewGameButton->setPosition( CEGUI::UVector2( CEGUI::UDim(0.4f,0), CEGUI::UDim(0.4f,0) ) );
	m_pSheet->addChildWindow(m_pNewGameButton);
	m_pNewGameButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&App::NewGame, this));
	
	//make a quit button
	m_pQuit = wmgr.createWindow("TaharezLook/Button", "CEGUI/QuitButton");
	m_pQuit->setText("Quit");
	m_pQuit->setSize(CEGUI::UVector2(CEGUI::UDim(0.2f, 0), CEGUI::UDim(0.1f, 0)));
	m_pQuit->setPosition( CEGUI::UVector2( CEGUI::UDim(0.4f,0), CEGUI::UDim(0.8f,0) ) );
	m_pSheet->addChildWindow(m_pQuit);
	m_pQuit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&App::Quit, this));
	
	//make a main menu button
	m_pMainMenuButton = wmgr.createWindow("TaharezLook/Button", "CEGUI/MainMenuButton");
	m_pMainMenuButton->setText("Back to Main Menu");
	m_pMainMenuButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.2f, 0), CEGUI::UDim(0.1f, 0)));
	m_pMainMenuButton->setPosition( CEGUI::UVector2( CEGUI::UDim(0.05f,0), CEGUI::UDim(0.05f,0) ) );
	m_pSheet->addChildWindow(m_pMainMenuButton);
	m_pMainMenuButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&App::MainMenu, this));
	m_pMainMenuButton->setVisible(false);
}
//-------------------------------------------------------------------------------------
bool App::MainMenu(const CEGUI::EventArgs &e)
{
	//problem with my cleanup code; it seems to crash every time i recreate the scene?
	destroyScene();
	m_pQuit->setPosition( CEGUI::UVector2( CEGUI::UDim(0.4f,0), CEGUI::UDim(0.8f,0) ) );
	//
	m_pNewGameButton->setVisible(true);
	m_pTitle->setVisible(true);
	m_pMainMenuButton->setVisible(false);
	//
	return true;
}
//-------------------------------------------------------------------------------------
bool App::NewGame(const CEGUI::EventArgs &e)
{
	createScene();
	//
	return true;
}
//-------------------------------------------------------------------------------------
bool App::Quit(const CEGUI::EventArgs &e)
{
	mShutDown = true;
	return true;
}
//-------------------------------------------------------------------------------------
void App::ThrowShuriken()
{
	m_pThrownShuriken = new Shuriken("Shuriken", *mSceneMgr, mCamera);
	Ogre::Vector3 startPos = m_pPlayerEntity->GetPosition();
	startPos.y += 25;
	m_pThrownShuriken->SetPosition(startPos);
}
//-------------------------------------------------------------------------------------
void App::ShurikenDie()
{
	//
}
//-------------------------------------------------------------------------------------