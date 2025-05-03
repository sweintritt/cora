import os
import shutil
from distutils.command.clean import clean as Clean

from setuptools import setup


class CleanCommand(Clean):
    def run(self):
        Clean.run(self)
        folders_to_remove = [
            './build',
            './dist',
            './src/cora.egg-info'
        ]

        for folder in folders_to_remove:
            shutil.rmtree(folder, ignore_errors=True)

        for root, dirs, files in os.walk('.'):
            for dir_ in dirs:
                if dir_ == '__pycache__':
                    shutil.rmtree(os.path.join(root, dir_), ignore_errors=True)
            for file in files:
                if file.endswith('.pyc'):
                    os.remove(os.path.join(root, file))


setup(
    cmdclass={'clean': CleanCommand},
)
