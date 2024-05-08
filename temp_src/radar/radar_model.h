#include <vector>
#include <string>
#include <optional>


typedef struct {

    double x;
    double y;

} vertex_2d;

typedef struct {

    vertex_2d v[2];

} matrix_2d;


class radar_model {

    //prefixes: w_ = world, m_ = model, r_ = rotated

    //attributes
    private:
    double max_x, max_y;
    
    vertex_2d m_origin, r_origin;
    std::vector<vertex_2d> m_players, r_players;
    
    double theta;
    matrix_2d r_matrix;

    //methods
    private:
    void world_to_model(std::vector<vertex_2d> * new_w_players);
    void init_r_matrix();
    void rotate_m_players();

    public:
    radar_model(double max_x, double max_y);
    
    std::optional<std::string> update(vertex_2d new_m_origin, std::vector<vertex_2d> new_w_players, double theta);
};
