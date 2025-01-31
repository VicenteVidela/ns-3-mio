import csv, os, json
from pathlib import Path

def parse_semmicolon_to_list(str_rep):
    return ((str_rep.replace("[", "")).replace("]", "")).split(";")


class ResultsReader:

    def __init__(self):
        self.fraction_of_physical_nodes_removed_list = []
        self.physical_node_removed_list = []
        self.logical_nodes_lost_list = []
        self.physical_nodes_lost_list = []
        self.g_l_list = []

    def get_fraction_of_physical_nodes_removed(self):
        return self.fraction_of_physical_nodes_removed_list

    def get_physical_node_removed_per_iteration(self):
        return self.physical_node_removed_list

    def get_logical_nodes_lost_list(self):
        return self.logical_nodes_lost_list

    def get_physical_nodes_lost_list(self):
        return self.physical_nodes_lost_list

    def get_gl_list(self):
        return self.g_l_list

    def load_from_file(self, file_path):
        """Carga la informacion desde un archivo en el formato horrible que hice."""
        with open(file_path, newline='') as csvfile:
            csvreader = csv.reader(csvfile, delimiter=',')
            first_row = None
            for row in csvreader:
                if first_row is None:
                    first_row = row
                    continue
                self.fraction_of_physical_nodes_removed_list.append(float(row[0]))
                self.physical_node_removed_list.append(row[1])
                self.logical_nodes_lost_list.append(parse_semmicolon_to_list(row[2]))
                self.physical_nodes_lost_list.append(parse_semmicolon_to_list(row[3]))
                try:
                    self.g_l_list.append(float(row[4]))
                except ValueError:
                    self.g_l_list.append(0.0)

    def get_list_of_logical_nodes_lost_in_order(self):
        """Entrega una lista ordenada de los nodos lógicos que se perdieron en el proceso. Cada elemento de la lista contiene una lista con los nodos
        que se perdieron en una iteración. Se omiten las iteraciones donde remover nodos fisicos NO resultó en la pérdida de remover nodos lógicos.
        """
        ordered_list = []
        for logical_nodes_lost_list in self.logical_nodes_lost_list:
            if len(logical_nodes_lost_list) > 0 and logical_nodes_lost_list[0] != '':
                ordered_list.append(logical_nodes_lost_list)
        return ordered_list

    def get_list_of_logical_nodes_lost_in_order_and_gl(self):
        """Entrega una lista ordenada de los nodos lógicos que se perdieron en el proceso y una lista con los GL asociados. Cada elemento de la lista
        contiene una lista con los nodos que se perdieron en una iteración. Se omiten las iteraciones donde remover nodos fisicos NO resultó en la pérdida
        de remover nodos lógicos.
        """
        physical_nodes_lost_ordered_list = []
        gl_ordered_list = []
        i = 0
        for logical_nodes_lost_list in self.logical_nodes_lost_list:
            if len(logical_nodes_lost_list) > 0 and logical_nodes_lost_list[0] != '':
                physical_nodes_lost_ordered_list.append(logical_nodes_lost_list)
                gl_ordered_list.append(self.g_l_list[i])
            i += 1
        return physical_nodes_lost_ordered_list, gl_ordered_list

    def get_list_of_fraction_of_physical_nodes_removed_and_gl(self):
        """Entrega una lista ordenada de la fracción de nodos físicos removidos y una lista con los GL asociados. Cada elemento de la lista
        contiene la fracción de nodos físicos rem"""
        fraction_dictionary = {}
        for i, j in zip(self.g_l_list, self.fraction_of_physical_nodes_removed_list):
            if i not in fraction_dictionary:
                fraction_dictionary[i] = []
            fraction_dictionary[i].append(j)
        return fraction_dictionary

# Define directories
input_directory = Path('ER_50_iteraciones/')
output_directory = Path('output/')
output_directory_fraction = Path('output_fraction/')
# Create output directory if it doesn't exist
os.makedirs(output_directory, exist_ok=True)

for file_path in input_directory.rglob('*'):
    if file_path.is_file():
        # Recreate the same relative path in the destination folder
        relative_path = file_path.relative_to(input_directory).with_suffix(".disc")
        new_file_path = output_directory / relative_path

        relative_path_fraction = file_path.relative_to(input_directory).with_suffix(".fraction")
        new_file_path_fraction = output_directory_fraction / relative_path_fraction

        # Create the parent directories if they don't exist
        new_file_path.parent.mkdir(parents=True, exist_ok=True)
        new_file_path_fraction.parent.mkdir(parents=True, exist_ok=True)

        rr = ResultsReader()
        rr.load_from_file(file_path)
        data = rr.get_list_of_logical_nodes_lost_in_order_and_gl()
        with open(new_file_path, 'w') as output_file:
            output_file.write(f"{data[0]}\n")
            # Write the second list as a list of floats
            output_file.write(f"{data[1]}")

        data_fraction = rr.get_list_of_fraction_of_physical_nodes_removed_and_gl()
        with open(new_file_path_fraction, 'w') as output_file:
            output_file.write(json.dumps(data_fraction))




# rr = ResultsReader()
# rr.load_from_file('datos/20x500/ndep5/5NN/AV_it10_result_ppv3_lv1_20x500_exp_2.5_ndep_5_att_physical_v10_m_5NN_n1.csv')
# print(rr.get_list_of_logical_nodes_lost_in_order_and_gl())