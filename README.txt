Bài tập toán rời rạc đường đi của robot
Ý tưởng của giải thuật sử dụng trong bài:
+ Kết hợp giải thuật dijkstra được học trên lớp
+ Ý tưởng giải bài toán coi trạng thái vị trí của 2 robot là 1 đỉnh của đồ thị (vd robot 1 ở 1, robot2 ở 3)
-> 1 đỉnh "1 3" (kiểu char) 
==> Các trạng thái có thể đến từ 1 trạng thái đỉnh là hàng xóm
lưu vào trong đồ thị
+ Mục tiêu đến trạng thái đích
##

Qua chạy thử 3 bộ test1.txt test2.txt test3.txt

Nhận thấy so với kết quả của thầy khác nhưng vẫn đảm bảo
đúng yêu cầu đề bài

Cách chạy:

gcc -g -c dijkstra.c
gcc -g -o test dijkstra.c libfdr.a
./test test2.txt

##Lưu ý: test2.txt là bộ txt muốn kiểm thử
Hết.
