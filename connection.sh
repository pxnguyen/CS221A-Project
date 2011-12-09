rm "connection.results"
for i in `seq 1 100`;
do
	./client 127.0.0.1 >> connection.results
done
