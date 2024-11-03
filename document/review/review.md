# Review


- `auto mux = _bump_to_hori_muxs[group];`：`auto` or `auto&` 
- 这三个 API 应该设计的一致吧：不过我还没看
  ````c++
  auto hori_mux_reg_value(std::usize index) -> std::Option<std::usize>;
  auto vert_mux_reg_value(std::usize index) -> std::Option<std::usize>;
  // ??
  auto track_mux_reg_value(std::usize index) -> std::usize;
  ````
- 返回值优化
  ````c++
  auto Builder::create_interposer() -> std::Box<hardware::Interposer> {
      return std::move(std::make_unique<hardware::Interposer>());
  }
  ````
- 参数传递 move
  ````c++
  auto Builder::add_topdies_to_basedie() -> void
  try {
      auto& topdies_config = _config.topdies;

      auto* p_topdies {new std::HashMap<std::String, circuit::TopDie>()};
      for (auto& topdie_config: topdies_config)
      {
          const std::String& topdie_name {topdie_config.first};
          auto& pins_map {topdie_config.second.pin_map};
          circuit::TopDie* p_topdie {new circuit::TopDie(topdie_name, pins_map)};
          p_topdies->emplace(topdie_name, *p_topdie);
      }
      _basedie->set_topdies(*p_topdies);
  }
  catch(std::exception& e) {
      throw std::runtime_error(std::String(e.what()));
  }

  // ...

  auto set_topdies(std::HashMap<std::String, TopDie>& other) -> void {this->_topdies = other;}
  ````
  ````c++
  TopDie::TopDie(std::String name, std::HashMap<std::String, std::usize> pins_map) :
    _name{std::move(name)}, 
    _pins_map{std::move(pins_map)}
  {
      
  }
  ````