// sim.cpp
void InitScene( void );
void UpdateScene( void );

// draw.cpp
void DrawScene( void );

// projview.cpp
void Viewing( void );
void Projection( void );


#define NUM_BALLOON 100

//-------- common window parameters
typedef struct {
    int xo;
    int yo;
    int width;
    int height;
    float aspect;
	char *title;
} WindowDataT;

//// オブジェクト用の共通構造体
typedef struct {
	float x, y, z; //位置座標
	float roll, pitch, yaw; //姿勢（回転）情報
	bool visuable;
	int state; //状態変数
} ObjDataT;

//// 風船用の共通構造体
typedef struct {
	float x, y, z; //位置座標
	float roll, pitch, yaw; //姿勢（回転）情報
	float rad;  //風船の半径
	int state; //状態変数
	bool visuable[NUM_BALLOON];
} BalloonDataT;

//-------- simulation data
typedef struct {
    //////// ユーザ定義のシーンについてのデータをここにまとめる
	ObjDataT camera0; //カメラ0オブジェクト(主観視点）
	ObjDataT camera1; //カメラ1オブジェクト(客観視点）
	ObjDataT camera;
	ObjDataT player;

	float move; //カメラの移動量
	float rot; //カメラの回転量

	bool mouse_left;    //マウスの左ボタンの状態
	bool mouse_right;  //マウスの右ボタンの状態
	bool mouse_middle;  //マウスの中ボタンの状態


	float mouse_x; //正規化されたマウスのX座標[-1.0, 1.0](右側が正)
	float mouse_y; //正規化されたマウスのY座標[-1.0, 1.0](左側が正)

	float playerdist;  //オブジェクト中心間距離
	

	int ballooncount;     //風船の数の計算
	int player_state;   //プレイヤーのy軸の位置
	float player_dia;  //プレイヤーオブジェクトの直径
	



	BalloonDataT balloon[NUM_BALLOON];  // 風船の位置
	float getballoon[NUM_BALLOON][3];  //衝突時の風船の移動量
	int dist_state[NUM_BALLOON];  //風船の状態遷移　　1=風船の相対位置定義→　2=風船のplayerからの位置
	////////
} SimDataT;
