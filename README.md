# SE-Thesis-Experiments
Repository to host all experiments for Master Software Engineering Thesis with title: "Running R/C++ hybrid programs in Cloud as workflows". These experiments are used to test the effectiveness of the Hybrid Containerizer and to compare the performance of R with C++ through Rcpp and R and C++ in separate containers. The repository for the Hybrid Containerizer can be found at: https://github.com/SjoerdHulleman/Hybrid-Containerizer.

Each experiment in this repo has two versions, "Original" and "Separate containers". "Original" is the original R script before being fed to the Hybrid Containers, and "Separate containers" is after it is fed to the Hybrid Containerizer.

Included experiments:
- Baseline: an experiment to test the base overhead of R with C++ through Rcpp and R and C++ in separate containers
- NIOZ: an experiment for sea research supplied by the Royal Netherlands Institute for Sea Research
- MergeSort: an implementation of mergesort, has varying input sizes for extra performance tests.
- MultiplyVectors: an implementation that multiplies two input vectors, has varying input sizes for extra performance tests.
- Backcross: calculates the recombination fraction in a backcross population. Modified version of code available at: https://github.com/mmollina/Cpp_and_R.

To build a C++/R image, use the following command:
```
docker build -t IMAGE_NAME .
```
To run a C++ container, use the following command:
```
docker run --rm --network host IMAGE_NAME
```

To run an R container, use the following command:
```
docker run --rm --network host IMAGE_NAME -c "source /venv/bin/activate; Rscript /app/task.R"
```
If you are running the version with separate R and C++ containers, make sure the C++ container is already running since this container functions as an API for the R container.

These commands apply if the containers are running on the same machine. If this is not the case, other network settings apply.
