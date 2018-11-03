#ifndef SORTING_HPP
#define SORTING_HPP

template<typename T> void ShellSort(int n, T arr[])
{
	for (int gap = n / 2; gap > 0; gap /= 2)
	{
		for (int i = gap; i < n; i++)
		{
			T temp = arr[i];
			int j;

			for (j = i; j >= gap && arr[j - gap] > temp; j -= gap)
			{
				arr[j] = arr[j - gap];
			}
			arr[j] = temp;
		}
	}
}

#endif