#pragma once

namespace TD
{
template <typename T> class Manager
{
  public:
    static T &get_instance()
    {
        static T instance;
        return instance;
    }

  protected:
    Manager() = default;
    virtual ~Manager() = default;
  private:
    Manager(Manager const &)            = delete;
    Manager &operator=(Manager const &) = delete;
};

} // namespace TD
