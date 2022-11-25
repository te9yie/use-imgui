namespace {

class DebugGui {
 private:
  struct Drop {
    ~Drop() {
      SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "shutdown ImGui.");
      ImGui_ImplOpenGL3_Shutdown();
      ImGui_ImplSDL2_Shutdown();
      ImGui::DestroyContext();
    }
  };

 private:
  std::unique_ptr<Drop> drop_;

 public:
  DebugGui() : drop_(std::make_unique<Drop>()) {}

  void handle_event(const SDL_Event& e) { ImGui_ImplSDL2_ProcessEvent(&e); }

  void begin_frame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
  }

  void end_frame() { ImGui::Render(); }

  void render() { ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); }
};

inline t9::Result<DebugGui> build_debug_gui(SDL_Window* w, SDL_GLContext c) {
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "initialize ImGui.");

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGui_ImplSDL2_InitForOpenGL(w, c);
  ImGui_ImplOpenGL3_Init("#version 130");

  return DebugGui();
}

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
#if defined(_MSC_VER)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

  using namespace t9::sdl2;
  auto core = build_core().unwrap();
  auto video = core.video().unwrap();
  auto window = video.window("ImGui", 16 * 80, 9 * 80)
                    .flags(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)
                    .build()
                    .unwrap();
  auto gl = window.gl().build().unwrap();
  auto gui = build_debug_gui(window.into(), gl.into()).unwrap();

  SDL_GL_SetSwapInterval(1);

  bool loop = true;
  while (loop) {
    if (window.handle_event([&gui](const SDL_Event& e) {
          gui.handle_event(e);
        }) == HandleEventResult::Quit) {
      loop = false;
    }

    gui.begin_frame();

    ImGui::ShowDemoWindow();

    gui.end_frame();

    glClearColor(0.12f, 0.34f, 0.56f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    gui.render();
    gl.swap();
  }

  return 0;
}
